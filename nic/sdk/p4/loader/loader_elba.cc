/*
 * Copyright (c) 2020, Pensando Systems Inc.
 */

/*
 * p4_loader.cc: Implementation of APIs for MPU program loader
 */

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/unordered_map.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "p4/loader/loader.hpp"
#include "third-party/asic/ip/verif/pensim/src/lib/libmpuobj/include/libmpuobj.h"
#include "third-party/asic/ip/verif/pensim/src/lib/libelbisa/include/libelbisa.h"
#include "third-party/asic/ip/verif/pensim/src/lib/libelbisa/include/libelbmpuobj.h"
#include "asic/rw/asicrw.hpp"
#include "platform/utils/program.hpp"

namespace sdk {
namespace p4 {

namespace pt = boost::property_tree;

/* P4 loader's MPU program information structure */
typedef struct {
    std::string name;
    uint64_t base_addr;
    uint64_t size;
    ElbaMpuProgram prog;
    ElbaMpuProgram copy;
    MpuSymbolTable unresolved_params;
    MpuSymbolTable resolved_params;
    MpuSymbolTable labels;
} p4_program_info_t;

/* P4 loader's context structure */
typedef struct {
    std::string handle;
    uint64_t prog_hbm_base_addr;
    p4_program_info_t *program_info = NULL;
    int num_programs;
} p4_loader_ctx_t;


/* TODO: Declaring these as globals for now. Figure out usage and define
 *       these appropriately.
 */
boost::unordered_map<std::string, p4_loader_ctx_t *> loader_instances;

/**
 * read_programs: Read all the programs in a specified directory. For now
 *                assumes only MPU programs reside in that directory.
 *
 * @pathname: Fully specified path name of the directory which contains the
 *            MPU programs
 *
 * Return: 0 on success, < 0 on failure
 */
static int
read_programs(const char *handle, char *pathname, mpu_pgm_sort_t sort_func)
{
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    p4_loader_ctx_t *ctx;
    std::vector <std::string> program_names;

    /* Load context */
    ctx = loader_instances[handle];
    if (!ctx) {
        return -1;
    }

    if ((dir = opendir (pathname)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                program_names.push_back(ent->d_name);
            }
        }
        closedir (dir);
    } else {
        SDK_TRACE_ERR("Cannot open dir %s %s", pathname, strerror(errno));
        return -1;
    }

    ctx->program_info = new p4_program_info_t[program_names.size()];

    // sort the mpu program names
    if (sort_func) {
        sort_func(program_names);
    } else {
        std::sort(program_names.begin(), program_names.end());
    }

    for (auto it = program_names.cbegin(); it != program_names.cend(); it++) {
        ctx->program_info[i].name = *it;
        i++;
    }
    return i;
}

/**
 * param_check: Check to see if a param (speficied by its name) is present in
 *              the symbol table
 *
 * @prog_param_info: Program's parameter list
 * @prog_name: Parameter name to check
 * @val: Pointer to where the value is to be stored
 *
 * Return: Index of parameter on success, < 0 on failure
 */
static int
param_check(p4_param_info_t *symbols, int num_symbols, std::string param, uint64_t *val)
{
    int i;

    if (!symbols || num_symbols == 0) {
        return -1;
    }

    for (i = 0; i < num_symbols; i++) {
         if (param == symbols[i].name) {
             *val = symbols[i].val;
             return i;
         }
    }
    return -1;
}

/**
 * p4_load_mpu_programs: Load all MPU programs in a given directory. Resolve
 *                          the parameters defined in the programs using an
 *                          input list + by checking against labels defined in
 *                          the programs.  Finally write the programs to HBM
 *                          memory.
 *                          NOTE: For now the assumption is that directory
 *                                specified by "pathname" will contain only MPU
 *                                binaries.
 *
 * @pathname: Fully specified path name of the directory which contains the
 *            MPU programs
 * @hbm_base_addr: Base address in HBM to use when loading the MPU programs
 * @prog_param_info: symbol table <name, val>
 * @num_prog_params: Number of elements in the prog_param_info array
 *
 * Return: sdk_ret_t
 */
sdk_ret_t
p4_load_mpu_programs (const char *handle,
                      char *pathname, uint64_t hbm_base_addr,
                      p4_param_info_t *prog_param_info,
                      int num_prog_params, mpu_pgm_sort_t sort_func,
                      bool write_to_mem)
{
    int i, j;
    p4_loader_ctx_t *ctx;
    p4_program_info_t *program_info;
    MpuSymbol *symbol, *param_u, *param_r;
    uint64_t val;
    sdk_ret_t rv;
    MpuSymbolTable global_labels;
    sdk::platform::utils::program_info  *prog_info;

    /* ISA library initialization */
    if (libelbisa_init() < 0) {
        SDK_TRACE_ERR("Libelbisa initialization failed!");
        SDK_ASSERT_RETURN(0, SDK_RET_ERR);
    }

    /* Input check */
    if (!handle || !pathname) {
        SDK_TRACE_ERR("Input error");
        SDK_ASSERT_RETURN(0, SDK_RET_ERR);
    }

    /* Create a loader instance */
    ctx = loader_instances[handle];
    if (ctx) {
        SDK_TRACE_ERR("Programs already loaded!");
        SDK_ASSERT_RETURN(0, SDK_RET_ERR);
    }

    prog_info = program_info::factory();

    /* Allocate context */
    ctx = new p4_loader_ctx_t;
    ctx->handle = handle;
    // ctx->program_info = new p4_program_info_t[MAX_PROGRAMS];
    loader_instances[handle] = ctx;

    /* Read all program names */
    if ((ctx->num_programs = read_programs(handle, pathname, sort_func)) < 0) {
        SDK_TRACE_ERR("Cannot read programs");
        SDK_ASSERT_RETURN(0, SDK_RET_ERR);
    }
    program_info = ctx->program_info;
    SDK_TRACE_DEBUG("Num programs %u", ctx->num_programs);

    /* Other initializations */
    std::string path_str = pathname;
    ctx->prog_hbm_base_addr = (hbm_base_addr + 63) & 0xFFFFFFFFFFFFFFC0L;
    if ((ctx->prog_hbm_base_addr & 63) != 0) {
        SDK_TRACE_ERR("Invalid HBM base address");
        SDK_ASSERT_RETURN(0, SDK_RET_ERR);
    }

    /* Pass 1: Load all MPU programs into a data structure. Separate the symbols
     *         in those MPU programs into 3 categories:
     *         1. Resolved labels (maintained in a global list also)
     *         2. Resolved parameters (resolved through the input list)
     *         3. Unresolved parameters
     */
    for (i = 0; i < ctx->num_programs; i++) {
        /* Load the program from the ELF file and check for errors */
        std::string filename = path_str+ "/" + program_info[i].name;
        if (program_info[i].prog.load_from_elf(filename) < 0) {
            SDK_TRACE_ERR("Error %s, file %s",
               program_info[i].prog.errmap[program_info[i].prog.err].c_str(),
               filename.c_str());
            SDK_ASSERT_RETURN(0, SDK_RET_ERR);
        }

        /* Save the base address and size */
        program_info[i].base_addr = ctx->prog_hbm_base_addr;
        program_info[i].size = program_info[i].prog.text.size()*sizeof(uint64_t);

        prog_info->add_program(program_info[i].name, program_info[i].base_addr,
                               (((program_info[i].base_addr + program_info[i].size + 63) & 0xFFFFFFFFFFFFFFC0L) - 1));


        /* Dump program specific info and the symbol table */
        //SDK_TRACE_DEBUG("MPU Program %s loaded, valid %d, "
                        //"complete %d, number of symbols %lu, "
                        //"base address {%lx}, size %lu",
                        //program_info[i].name.c_str(),
                        //program_info[i].prog.valid,
                        //program_info[i].prog.complete,
                        //program_info[i].prog.symtab.size(),
                        //program_info[i].base_addr, program_info[i].size);

        /* Iterate through the program's symbol table */
        for (j = 0; j < (int) program_info[i].prog.symtab.size(); j++) {
            /* Get each symbol by its id */
            if ((symbol = program_info[i].prog.symtab.get_byid(j)) == NULL) {
                SDK_TRACE_ERR("Id %d, Cannot get symbol", j);
                SDK_ASSERT_RETURN(0, SDK_RET_ERR);
            } else {
                /* Symbol is a parameter */
                if (symbol->type == MPUSYM_PARAM) {
                    /* If <program, parameter> was passed in the input list =>
                     *    resolve the parameter's value based on the input.
                     * Else
                     *    add the parameter to the list of unresolved list
                     */
                    val = 0;
                    if (prog_param_info &&
                        param_check(prog_param_info, num_prog_params, symbol->name,
                                    &val) >= 0) {
                        //SDK_TRACE_DEBUG("Resolved param: prog_name %s name %s val {%lx}",
                                          //program_info[i].name.c_str(), symbol->name.c_str(), val);
                        program_info[i].resolved_params.add(
                            MpuSymbol(symbol->name.c_str(), MPUSYM_PARAM, val));
                    } else {
                        //SDK_TRACE_DEBUG("Unresolved param: name %s",
                                        //symbol->name.c_str());
                        program_info[i].unresolved_params.add(
                            MpuSymbol(symbol->name.c_str(), MPUSYM_PARAM, 0x0));
                    }
                /* Symbol is a label */
                } else if (symbol->type == MPUSYM_LABEL) {
                    /* Add it to the list of program specific and global labels
                     * which will help resolve unknown parameters in pass 2
                     */
                    //SDK_TRACE_DEBUG("label: name %s addr {%lx}",
                                    //symbol->name.c_str(),
                                    //program_info[i].base_addr + symbol->val);
                    global_labels.add(
                             MpuSymbol(symbol->name.c_str(),
                                       symbol->type,
                                       program_info[i].base_addr+ symbol->val));
                    program_info[i].labels.add(
                             MpuSymbol(symbol->name.c_str(),
                                       symbol->type, symbol->val));

                prog_info->add_symbol(program_info[i].name, symbol->name, symbol->val);

                /* Symbol type not known */
                } else {
                    /* Other symbol types are not supported at the moment*/
                    SDK_TRACE_ERR("Unknown symbol type %d", symbol->type);
                    SDK_ASSERT_RETURN(0, SDK_RET_ERR);
                }
            }
        }
        /* Increment the running counter of HBM base address */
        ctx->prog_hbm_base_addr += program_info[i].size;
        ctx->prog_hbm_base_addr = (ctx->prog_hbm_base_addr + 63) & 0xFFFFFFFFFFFFFFC0L;
    }

    /* Pass 2: Resolve all the unresolved parameters based on looking up the
     *         global labels
     */
    for (i = 0; i < ctx->num_programs; i++) {
        for (j = 0; j < (int) program_info[i].unresolved_params.size(); j++) {
            /* Get the unresolved parameter by id */
            if ((param_u = program_info[i].unresolved_params.get_byid(j))
                == NULL) {
                SDK_TRACE_ERR("Id %d, Cannot get unresolved param", j);
                SDK_ASSERT_RETURN(0, SDK_RET_ERR);
            } else {
                /* Try to resolve it by looking at the global labels */
                if ((param_r = global_labels.get_byname(param_u->name.c_str()))
                    != NULL) {
                    //SDK_TRACE_DEBUG("Resolved param %s to value {:%lx}",
                                      //param_r->name.c_str(), param_r->val);

                    /* Add it to the list of resolved params for that program */
                    program_info[i].resolved_params.add(
                           MpuSymbol(param_u->name.c_str(), MPUSYM_PARAM,
                                     param_r->val));
                } else {
                    SDK_TRACE_WARN("Cannot resolve param %s",
                                  param_u->name.c_str());
                }
            }
        }
    }

    /* Pass 3: Build a copy of the program based on the resolved parameters and
     *         write it to HBM
     */
    for (i = 0; i < ctx->num_programs; i++) {
       program_info[i].copy = ElbaMpuProgram(program_info[i].prog,
                                         program_info[i].resolved_params);
       /* Sanity check the size of the copy, valid and complete flags */
       if (program_info[i].copy.valid != 1 ||
           program_info[i].copy.complete != 1 ||
           program_info[i].copy.text.size()*sizeof(uint64_t) !=
           program_info[i].size) {
           SDK_TRACE_DEBUG("Failed to resolve program: name %s, "
                           "base address {:%lx}, size %lu, valid %d, complete %d",
                           program_info[i].name.c_str(),
                           program_info[i].base_addr,
                           program_info[i].size,
                           program_info[i].copy.valid,
                           program_info[i].copy.complete);

           SDK_TRACE_DEBUG("MPU symbol table ");
           program_info[i].copy.symtab.dump();
           SDK_TRACE_DEBUG("MPU reloctab ");
           program_info[i].copy.reloctab.dump();

           SDK_ASSERT_RETURN(0, SDK_RET_ERR);
       } else {
           //SDK_TRACE_DEBUG("Successfully resolved program: name %s, "
                           //"base address {:%lx}, size %lu, valid %d, complete %d",
                           //program_info[i].name.c_str(),
                           //program_info[i].base_addr,
                           //program_info[i].size,
                           //program_info[i].copy.valid,
                           //program_info[i].copy.complete);
       }

       if (write_to_mem) {
           /* Write program to HBM */
           rv = sdk::asic::asic_mem_write(program_info[i].base_addr,
                                          (uint8_t *) program_info[i].copy.text.data(),
                                          program_info[i].size,
                                          ASIC_WRITE_MODE_WRITE_THRU);
           if (rv != SDK_RET_OK) {
               SDK_TRACE_ERR("HBM program write failed");
               SDK_ASSERT_RETURN(0, SDK_RET_ERR);
           }
       }
    }

    return SDK_RET_OK;
}

void
p4_cleanup (void)
{
    p4_loader_ctx_t *ctx;

    for (auto it = loader_instances.begin(); it != loader_instances.end(); it++) {
        if ((ctx = loader_instances[it->first]) != NULL) {
            if (ctx) {
                if (ctx->program_info) {
                    delete[] ctx->program_info;
                }
                delete ctx;
                loader_instances[it->first] = NULL;
            }
        }
    }
}

/**
 * p4_program_label_to_offset: Resolve a programs, label to its relative
 *                                offset
 *
 * @prog_name: Program name
 * @label_name: Label name
 * @offset: Offset to be filled in if the program, label is found
 *
 * Return: sdk_ret_t
 */
sdk_ret_t
p4_program_label_to_offset(const char *handle,
                              char *prog_name, char *label_name,
                              uint64_t *offset)
{
    std::string prog_name_str = prog_name;
    int i;
    MpuSymbol *label;
    p4_loader_ctx_t *ctx;
    p4_program_info_t *program_info;

    /* Input check */
    if (!prog_name || !label_name || !offset || !handle) {
        SDK_TRACE_ERR("Input error");
        return SDK_RET_ERR;
    }

    /* Load context */
    ctx = loader_instances[handle];
    if (!ctx) {
        SDK_TRACE_ERR("Invalid handle");
        return SDK_RET_ERR;
    }
    program_info = ctx->program_info;

    /* Iterate through the list of programs and its labels until the label is
     * found
     */
    for (i = 0; i < ctx->num_programs; i++) {
        if (program_info[i].name == prog_name_str) {
            if ((label = program_info[i].labels.get_byname(label_name))
                 != NULL) {
                //SDK_TRACE_DEBUG("Resolved program name %s label name %s to "
                                  //"value {:%lx}", program_info[i].name.c_str(),
                                  //label->name.c_str(), label->val);
                *offset = label->val;
                return SDK_RET_OK;
            }
        }
    }

    //SDK_TRACE_ERR("Could not resolve program name %s label name %s",
                    //prog_name, label_name);
    return SDK_RET_ERR;
}

/**
 * p4_program_offset_to_label: Resolve a program, relative offset to a label
 *
 * @prog_name: Program name
 * @offset: Offset value
 * @label_name: Pointer to the location where label name is to be filled
 * @label_size: Size allocated by caller for the label_name pointer
 *
 * Return: sdk_ret_t
 */
sdk_ret_t
p4_program_offset_to_label(const char *handle,
                              char *prog_name, uint64_t offset,
                              char *label_name, size_t label_size)
{
    std::string prog = prog_name;
    int i, j;
    MpuSymbol *label;
    p4_loader_ctx_t *ctx;
    p4_program_info_t *program_info;

    /* Input check */
    if (!prog_name || !label_name || !handle) {
        SDK_TRACE_ERR("Input error");
        return SDK_RET_ERR;
    }

    /* Load context */
    ctx = loader_instances[handle];
    if (!ctx) {
        SDK_TRACE_ERR("Invalid handle");
        return SDK_RET_ERR;
    }
    program_info = ctx->program_info;

    /* Iterate through the list of programs and all offset associated with its
     * labels until the offset is found
     */
    for (i = 0; i < ctx->num_programs; i++) {
        if (program_info[i].name == prog) {
            for (j = 0; j < (int) program_info[i].labels.size(); j++) {
                if ((label = program_info[i].labels.get_byid(j)) != NULL) {
                    if (offset == label->val) {
                        SDK_TRACE_DEBUG("Resolved program name %s offset %lu to "
                                        "label name %s",
                                        program_info[i].name.c_str(),
                                        offset, label->name.c_str());
                        strncpy(label_name, label->name.c_str(), label_size);
                        return SDK_RET_OK;
                    }
                }
            }
        }
    }
    SDK_TRACE_ERR("Could not resolve program name %s offset %lu",
                  prog_name, offset);
    return SDK_RET_ERR;
}

/**
 * p4_program_to_base_addr: Resolve a program to its base address in HBM
 *
 * @prog_name: Program name
 * @base_addr: Pointer to the location where base address is to be filled
 *
 * Return: sdk_ret_t
 */
sdk_ret_t
p4_program_to_base_addr(const char *handle,
                           char *prog_name, uint64_t *base_addr)
{
    std::string prog = prog_name;
    int i;
    p4_loader_ctx_t *ctx;
    p4_program_info_t *program_info;

    /* Input check */
    if (!prog_name || !base_addr || !handle) {
        SDK_TRACE_ERR("Input error");
        return SDK_RET_ERR;
    }

    /* Load context */
    ctx = loader_instances[handle];
    if (!ctx) {
        return SDK_RET_ERR;
    }
    program_info = ctx->program_info;

    /* Iterate through the list of programs and if found, return its
     * base address
     */
    for (i = 0; i < ctx->num_programs; i++) {
        if (program_info[i].name == prog) {
            //SDK_TRACE_DEBUG("Resolved program name %s to base_addr {:%lx}",
                            //program_info[i].name.c_str(),
                            //program_info[i].base_addr);
            *base_addr = program_info[i].base_addr;
            return SDK_RET_OK;
        }
    }
    //SDK_TRACE_ERR("Could not resolve program name %s", prog_name);
    return SDK_RET_ERR;
}

sdk_ret_t
p4_dump_program_info (const char *cfg_path)
{
    struct stat                         st = { 0 };
    std::string                         prog_info_file;
    p4_loader_ctx_t                     *ctx;
    p4_program_info_t                   *program_info;
    pt::ptree                           root, programs, program;
    MpuSymbol                           *symbol;
    char                                numbuf[32];

    if (!loader_instances.size()) {
        return SDK_RET_ERR;
    }

    if (!cfg_path) {
        // write in the current dir
        prog_info_file = std::string(LDD_INFO_FILE_NAME);
    } else {
        std::string gen_dir = std::string(cfg_path) + std::string("/") +
                    std::string(LDD_INFO_FILE_RPATH);
        // check if the gen dir exists
        if (stat(gen_dir.c_str(), &st) == -1) {
            // doesn't exist, try to create
            if (mkdir(gen_dir.c_str(), 0755) < 0) {
                SDK_TRACE_ERR("Gen directory %s/ doesn't exist, failed to create one\n",
                        gen_dir.c_str());
                return SDK_RET_ERR;
            }
        } else {
            // gen dir exists, check if we have write permissions
            if (access(gen_dir.c_str(), W_OK) < 0) {
                // don't have permissions to create this directory
                SDK_TRACE_ERR("No permissions to create log file in %s\n", gen_dir.c_str());
                return SDK_RET_ERR;
            }
        }

        prog_info_file = gen_dir + std::string("/") + std::string(LDD_INFO_FILE_NAME);
    }

    for (auto it = loader_instances.begin(); it != loader_instances.end(); it++) {
        if ((ctx = loader_instances[it->first]) != NULL) {
            SDK_TRACE_DEBUG("Listing programs for handle name %s", it->first.c_str());
            program_info = ctx->program_info;
            for (int i = 0; i < ctx->num_programs; i++) {
                pt::ptree    symbols;
                program.put("name", program_info[i].name.c_str());
                program.put("base_addr", program_info[i].base_addr);
                snprintf(numbuf, sizeof(numbuf), "0x%lx",
                         program_info[i].base_addr);
                program.put("base_addr_hex", numbuf);
                program.put("end_addr",
                            ((program_info[i].base_addr +
                              program_info[i].size + 63) & 0xFFFFFFFFFFFFFFC0L) - 1);
                snprintf(numbuf, sizeof(numbuf), "0x%lx",
                         ((program_info[i].base_addr +
                           program_info[i].size + 63) & 0xFFFFFFFFFFFFFFC0L) - 1);
                program.put("end_addr_hex", numbuf);
                for (int j = 0; j < (int) program_info[i].prog.symtab.size(); j++) {
                    pt::ptree    sym;
                    symbol = program_info[i].prog.symtab.get_byid(j);
                    if ((symbol != NULL) && (symbol->type == MPUSYM_LABEL)) {
                        sym.put("name", symbol->name.c_str());
                        sym.put("addr", symbol->val);
                        snprintf(numbuf, sizeof(numbuf), "0x%lx",
                                 symbol->val);
                        sym.put("addr_hex", numbuf);
                        symbols.push_back(std::make_pair("", sym));
                        sym.clear();
                    }
                }
                program.add_child("symbols", symbols);
                symbols.clear();
                for (int j = 0; j < (int)program_info[i].resolved_params.size(); j++) {
                    pt::ptree    sym;
                    symbol = program_info[i].resolved_params.get_byid(j);
                    if ((symbol != NULL) && (symbol->type == MPUSYM_PARAM)) {
                        sym.put("name", symbol->name.c_str());
                        sym.put("addr", symbol->val);
                        snprintf(numbuf, sizeof(numbuf), "0x%lx", symbol->val);
                        sym.put("addr_hex", numbuf);
                        symbols.push_back(std::make_pair("", sym));
                        sym.clear();
                    }
                }
                program.add_child("params", symbols);
                programs.push_back(std::make_pair("", program));
                program.clear();
                symbols.clear();
            }
        } else {
            SDK_TRACE_DEBUG("Cannot list programs for handle name %s",
                            it->first.c_str());
        }
    }

    root.add_child("programs", programs);
    pt::write_json(prog_info_file, root);

    return SDK_RET_OK;
}


}    // namespace p4
}    // namespace sdk
