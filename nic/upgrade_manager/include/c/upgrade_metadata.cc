#include <stdio.h>
#include <iostream>
#include "boost/property_tree/json_parser.hpp"
#include "upgrade_metadata.hpp"
#include "nic/upgrade_manager/utils/upgrade_log.hpp"

namespace upgrade {

using boost::property_tree::ptree;
using namespace std;

bool GetUpgCtxTablesFromMeta(UpgCtx& ctx,
                             string metafile,
                             unordered_map<string, TableMeta>& tables,
                             unordered_map<string, ComponentMeta>& comps) {
    ptree             root;

    std::ifstream json_cfg(metafile.c_str());
    read_json(json_cfg, root);
    try {
        for (ptree::value_type table : root.get_child("tables")) {
            TableMeta tableMeta;
            memset(&tableMeta, 0, sizeof(TableMeta));
            tableMeta.name = table.second.get<std::string>("name");
            tableMeta.version = table.second.get<int>("version");
            tables[tableMeta.name] = tableMeta;
        }
        for (ptree::value_type comp : root.get_child("components")) {
            ComponentMeta componentMeta;
            memset(&componentMeta, 0, sizeof(componentMeta));
            componentMeta.name = comp.second.get<std::string>("name");
            componentMeta.version = comp.second.get<int>("version");
            comps[componentMeta.name] = componentMeta;
        }
    } catch (std::exception const& e) {
        UPG_LOG_DEBUG("Unable to parse upgrade_metadata.json %s", e.what());
        return false;
    }
    return true;
}

bool GetUpgCtxFromMeta(UpgCtx& ctx) {
    string metafile = "/sw/nic/upgrade_manager/meta/upgrade_metadata.json";
    GetUpgCtxTablesFromMeta(ctx, metafile, ctx.preUpgTables, ctx.preUpgComps);
    //TODO: Fill postUpgTable
    return true;
}

}
