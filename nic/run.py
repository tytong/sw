#!/usr/bin/python

import os
import sys
import argparse
import socket
import re
import time
import signal
import atexit
import pdb
import glob
import shlex
from pwd import getpwnam


from subprocess import Popen, PIPE, call

# git clone https://github.com/pensando/sw
# cd sw && git submodule update --init

# Globals

#nic_dir = os.environ.get('WS_TOP')
asic = os.environ.get('ASIC')
if not asic:
    print('run.py: ASIC is not set, default to capri.')
    asic = 'capri'
nic_dir = os.path.dirname(sys.argv[0])
nic_dir = os.path.abspath(nic_dir)
print "WS_TOP = ", nic_dir
if nic_dir is None:
    print "WS_TOP is not set!"
    sys.exit(1)

pid = os.getpid()
hal_process = None
model_process = None
stg_dol_process = None
nw_dol_process = None
nicmgr_process = None
platform_model_server_process = None

build_log = nic_dir + "/build.log"
model_log = nic_dir + "/model.log"
hal_log = nic_dir + "/hal.log"
dol_log = nic_dir + "/dol.log"
stg_dol_log = nic_dir + "/stg_dol.log"
mbt_log = nic_dir + "/mbt.log"
storage_dol_log = nic_dir + "/storage_dol.log"
sample_client_log = nic_dir + "/sample_client.log"
bullseye_model_cov_file = nic_dir + "/coverage/bullseye_model.cov"
bullseye_hal_cov_file = nic_dir + "/coverage/sim_bullseye_hal.cov"
halctl_dir = nic_dir + "/agent/cmd/halctl"

naples_container_image_dir = nic_dir + "/obj/images"
#This should be changed to pick up the current version automatically.
naples_container_image = "naples-release-v1.tgz"
naples_container_name = "naples-v1"
naples_container_image_name = "pensando/naples:v1"
naples_agent_timeout = 600
naples_hal_timeout = 600
naples_sim_timeout = 600

#Pritn cores script
print_cores_script = nic_dir + "/tools/print-cores.sh"
#Naples start script
naples_container_startup_script = nic_dir + "/sim/naples/start-naples-docker.sh"

lock_file = nic_dir + "/.run.pid"

ms_libs = nic_dir + "/third-party/metaswitch/output/x86_64/"
# Environment

os.environ["LD_LIBRARY_PATH"] = "/usr/local/lib:/usr/local/lib64:asic/capri/model/capsim-gen/lib:third-party/lkl/export/bin:" + ms_libs
os.environ["PKG_CONFIG_PATH"] = "/usr/local/lib/pkgconfig"

#Path and executables
bin_dir = nic_dir + '/build/x86_64/iris/' + asic + '/bin/'
if asic == 'elba':
    model_name = "elb_model"
else:
    model_name = "cap_model"

model_executable = bin_dir + model_name

model_core_path  = nic_dir

hal_executable = bin_dir + "hal"
hal_core_path = nic_dir

def print_core(executable, core):
    cmd = ['gdb', '-batch', '-ex', 'bt']
    cmd.extend([executable, core])
    call(" ".join(cmd), universal_newlines=True, shell=True)

def get_latest_core_file(path):
    list_of_files = glob.glob(path + "/core.*")
    if list_of_files:
        latest_file = max(list_of_files, key=os.path.getctime)
        return latest_file

def remove_core_files(path):
    for filename in glob.glob(path + "/core.*"):
        os.remove(filename)

def remove_all_core_files():
    remove_core_files(hal_core_path)
    remove_core_files(model_core_path)

def process_core(executable, core_file):
    if os.path.isfile(core_file):
        #Wait for all std out to clear.
        time.sleep(5)
        print_core(executable, core_file)
        os.system("mv " + core_file + "  " + nic_dir + "/core." +  os.path.basename(executable))


def process_hal_core(hal_core_file):
    print ("**********HAL CORE BEGIN**********")
    process_core(hal_executable, hal_core_file)
    print ("**********HAL CORE END**********")

def process_model_core(model_core_file):
    print ("**********MODEL CORE BEGIN**********")
    process_core(model_executable, model_core_file)
    print ("**********MODEL CORE END**********")


# build
def build():
    print "* Starting build"
    print "- Log file: " + build_log
    start_time = time.time()

    nic_dir = nic_dir

    os.environ["LD_LIBRARY_PATH"] = "/usr/local/lib:/usr/local/lib64"
    os.environ["PKG_CONFIG_PATH"] = "/usr/local/lib/pkgconfig"
    os.chdir(nic_dir)

    log = open(build_log, "w")
    p = Popen(["make", "-j", "8"], stdout=log, stderr=log)
    p.communicate()
    log.close()

    print("- Build time: %s seconds\n" % round(time.time() - start_time, 2))

    return p.returncode

# ASIC model

def run_rtl(args):
    global model_log
    if args.test_suf:
        model_log = nic_dir + "/logs_%s/model.log" % args.test_suf
    log = open(model_log, "w")
    asic_src = os.getcwd() + "/asic"
    os.environ["ASIC_SRC"] = asic_src
    os.environ["LD_LIBRARY_PATH"] = ".:../libs:/usr/local/lib:/usr/local/lib64:" + os.getcwd() + "/asic/capri/model/capsim-gen/lib:/home/asic/bin/tools/lib64"
    os.environ["PATH"] = os.getcwd() + "/asic/common/tools/bin" + ":" + os.environ["PATH"]

    if args.port_mode == 'nomac':
        model_test = "core_basic_dol_2x100_ahbm"
    elif args.port_mode == '2x100':
        model_test = "core_basic_dol_2x100"
    elif args.port_mode == '8x25':
        model_test = "core_basic_dol_8x25"
    else:
        print "Unknown port_mode", args.port_mode
        sys.exit(1)

    coverage_opts = []
    if args.rtl_coverage:
        coverage_opts = [ '-cov', '-tcov' ]

    one_pkt_mode = ""
    if args.model_test:
        model_test = args.model_test
    flow_stat_tbl_base = 0x108faf800
    if args.storage:
        flow_stat_tbl_base += 0x4
    if not args.skipverify:
        one_pkt_mode = "+dol_one_pkt_mode=1 +save_rtl_pkts=1"
    model_cmd = [ 'runtest', '-ngrid', '-test', model_test, '-run_args', ' %s  +flow_stat_tbl_base=%s +dol_poll_time=5 +dump_axi +pcie_all_lif_valid=1 +UVM_VERBOSITY=UVM_MEDIUM +fill_pattern=0 +te_dbg +plog=info +mem_verbose +verbose +PLOG_MAX_QUIT_COUNT=100 +top_sb/initial_timeout_ns=60000 %s ' % (one_pkt_mode, flow_stat_tbl_base, args.runtest_runargs) ] + coverage_opts
    if args.noverilog:
        model_cmd = model_cmd + ['-ro']
    if not args.no_asic_dump:
        model_cmd = model_cmd + [ '-ucli', 'ucli_core' ]
    if args.test_suf:
        model_cmd = model_cmd + ['-test_suf', args.test_suf ]
    print os.getcwd() + "/asic/capri/verif/top/env"
    p = Popen(model_cmd, stdout=log, stderr=log, cwd = os.getcwd() + "/asic/capri/verif/top/env")
    print "* Starting ASIC rtl: pid (" + str(p.pid) + ")"
    print "- Log file: " + model_log + "\n"

    global model_process
    model_process = p
    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()

def run_model(args):
    global model_log
    if args.test_suf:
        model_log = nic_dir + "/logs_%s/model.log" % args.test_suf
    os.environ["LD_LIBRARY_PATH"] = ".:/usr/local/lib:/usr/local/lib64:/home/asic/bin/tools/lib64"
    model_cmd = [ "./"+model_name, "+PLOG_MAX_QUIT_COUNT=0", "+plog_add_scope=axi_trace" ]
    if args.modellogs:
        model_cmd.append("+plog=info")
        if args.gft or args.gft_gtest:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/gft/" + asic + "/gen/p4gen/gft/dbg_out/model_debug.json")
        elif args.apollo_gtest or args.apollo_scale_test or args.apollo_scale_vxlan_test:
            os.system("%s/tools/merge_model_debug.py --pipeline apollo --p4 apollo --rxdma p4plus_rxdma --txdma p4plus_txdma" % nic_dir)
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/apollo/" + asic + "/gen/p4gen/apollo/dbg_out/combined_model_debug.json")
        elif args.artemis_gtest or args.artemis_scale_test:
            os.system("%s/tools/merge_model_debug.py --pipeline artemis --p4 artemis --rxdma p4plus_rxdma --txdma p4plus_txdma" % nic_dir)
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/artemis/" + asic + "/gen/p4gen/artemis/dbg_out/combined_model_debug.json")
        elif args.athena_app:
            os.system("%s/tools/merge_model_debug.py --pipeline athena --p4 athena --rxdma p4plus_rxdma --txdma p4plus_txdma" % nic_dir)
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/athena/" + asic + "/gen/p4gen/athena/dbg_out/combined_model_debug.json")
        elif args.athena_gtests:
            os.system("%s/tools/merge_model_debug.py --pipeline athena --p4 athena --rxdma p4plus_rxdma --txdma p4plus_txdma" % nic_dir)
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/athena/" + asic + "/gen/p4gen/athena/dbg_out/combined_model_debug.json")
        elif args.apulu_gtest:
            os.system("%s/tools/merge_model_debug.py --pipeline apulu --p4 apulu --rxdma p4plus_rxdma --txdma p4plus_txdma" % nic_dir)
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/apulu/" + asic + "/gen/p4gen/apulu/dbg_out/combined_model_debug.json")
        elif args.hello_gtest:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/hello/" + asic + "/gen/p4gen/hello/dbg_out/model_debug.json")
        elif args.l2switch_gtest:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/l2switch/" + asic + "/gen/p4gen/l2switch/dbg_out/model_debug.json")
        elif args.elektra_gtest:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/elektra/" + asic + "/gen/p4gen/elektra/dbg_out/model_debug.json")
        elif args.phoebus_gtest:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/phoebus/" + asic + "/gen/p4gen/phoebus/dbg_out/model_debug.json")
        elif args.gft16_gtest:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/gft16/" + asic + "/gen/p4gen//gft16/dbg_out/model_debug.json")
        else:
            model_cmd.append("+model_debug=" + nic_dir + "/build/x86_64/iris/" + asic + "/gen/p4gen/p4/dbg_out/model_debug.json")
    if args.coveragerun or args.asmcov:
        dump_file= nic_dir + "/coverage/asm_cov.dump"
        model_cmd.append("+mpu_cov_dump_file=" + dump_file)
        dump_dir= nic_dir + "/coverage/asm_out_all"
        model_cmd.append("+mpu_pc_cov_dump_dir=" + dump_dir)

    global bin_dir
    if args.gft or args.gft_gtest:
        bin_dir = nic_dir + '/build/x86_64/gft/' + asic + '/bin/'
    elif args.apollo_gtest or args.apollo_scale_test or args.apollo_scale_vxlan_test:
        bin_dir = nic_dir + '/build/x86_64/apollo/' + asic + '/bin/'
    elif args.artemis_gtest or args.artemis_scale_test:
        bin_dir = nic_dir + '/build/x86_64/artemis/' + asic + '/bin/'
    elif args.apulu_gtest:
        bin_dir = nic_dir + '/build/x86_64/apulu/' + asic + '/bin/'
    elif args.athena_app:
        bin_dir = nic_dir + '/build/x86_64/athena/' + asic + '/bin/'
    elif args.athena_gtests:
        bin_dir = nic_dir + '/build/x86_64/athena/' + asic + '/bin/'
    elif args.l2switch_gtest:
        bin_dir = nic_dir + '/build/x86_64/l2switch/' + asic + '/bin/'
    elif args.elektra_gtest:
        bin_dir = nic_dir + '/build/x86_64/elektra/' + asic + '/bin/'
    elif args.phoebus_gtest:
        bin_dir = nic_dir + '/build/x86_64/phoebus/' + asic + '/bin/'
    elif args.gft16_gtest:
        bin_dir = nic_dir + '/build/x86_64/gft16/' + asic + '/bin/'
    elif args.hello_gtest:
        bin_dir = nic_dir + '/build/x86_64/hello/' + asic + '/bin/'

    os.chdir(bin_dir)
    log = open(model_log, "w")
    model_env = os.environ.copy()
    model_env["COVFILE"] = bullseye_model_cov_file
    p = Popen(model_cmd, preexec_fn=demote, env=model_env, stdout=log, stderr=log)

    #p = Popen(model_cmd, stdout=log, stderr=log, env=model_env)
    print "* Starting ASIC model pid (" + str(p.pid) + ")"
    print "- Log file: " + model_log + "\n"

    global model_process
    model_process = p
    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()

    log2 = open(model_log, "r")
    loop = 1

    # Wait until model is up and running

    #while loop == 1:
    #    for line in log2.readlines():
    #        if "initialized" in line:
    #            loop = 0
    log2.close()

#    log.close()

# HAL
def wait_for_hal_and_fte(timeout=600):
     print "Waiting for hal and fte to be UP..."
     sys.stdout.flush()
     hal_up = False
     fte_up = False
     t_end = time.time() + timeout
     while not hal_up or not fte_up:
         #print "Check files: " + str(glob.glob(nic_dir + "/hal.log*"))
         for filename in  glob.glob(nic_dir + "/hal.log*"):
             #print "Checking in : " + filename
             log2 = open(filename, "r")
             if not hal_up or not fte_up:
                 for line in log2.readlines():
                     #print "Checking in line: " + line
                     if "listening on" in line:
                         hal_up = True
                     if "Starting FTE instance" in line:
                         fte_up = True
                     if hal_up and fte_up:
                         print "Both hal_up " + str(hal_up) + " and fte_up: " + \
                             str(fte_up) + " are UP"
                         sys.stdout.flush()
                         return
             log2.close()
         time.sleep(2)
         print ".",
         sys.stdout.flush()
         if t_end and time.time() > t_end:
             print "Timeout waiting for HAL : " + str(hal_up) + " and FTE : " + str(fte_up)
             raise Exception("Timeout Waiting for HAL %d and FTE : %d" %(hal_up, fte_up))

def wait_for_hal(timeout=600):
     print "Waiting for hal to be UP..."
     sys.stdout.flush()
     hal_up = False
     t_end = time.time() + timeout
     while not hal_up:
         #print "Check files: " + str(glob.glob(nic_dir + "/hal.log*"))
         for filename in  glob.glob(nic_dir + "/hal.log*"):
             #print "Checking in : " + filename
             log2 = open(filename, "r")
             if not hal_up:
                 for line in log2.readlines():
                     #print "Checking in line: " + line
                     if "listening on" in line:
                         hal_up = True
                     if hal_up:
                         print "hal is up"
                         sys.stdout.flush()
                         return
             log2.close()
         time.sleep(2)
         print ".",
         sys.stdout.flush()
         if t_end and time.time() > t_end:
             print "Timeout waiting for HAL : " + str(hal_up)
             raise Exception("Timeout Waiting for HAL %d" %(hal_up))

def demote():
    if 'SUDO_USER' in os.environ:
        #model/hal cannot be started with running as sudo user as pthread real time thread will fail.
        uid = getpwnam(os.environ['SUDO_USER']).pw_uid
        gid = getpwnam(os.environ['SUDO_USER']).pw_gid
        os.setgid(gid)
        os.setuid(uid)
        os.environ['USER'] = (os.environ['SUDO_USER'])

def create_device_conf(fname, mode):
    f = open(fname, "w+")
    f.write("{\n")
    f.write("\"forwarding-mode\": \"%s\",\n" % (mode))
    f.write("\"feature-profile\": 1,\n")
    f.write("\"port-admin-state\": \"PORT_ADMIN_STATE_ENABLE\",\n")
    f.write("\"mgmt-if-mac\": 0\n")
    f.write("}\n")
    f.close()

def run_hal(args):
    snort_dir = nic_dir + "/hal/third-party/snort3/export"
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    os.environ["LD_LIBRARY_PATH"] = "/home/asic/bin/tools/lib64:" + os.environ["LD_LIBRARY_PATH"]
    #os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/../bazel-bin/nic/model_sim/" + ":" + snort_dir + "/x86_64/lib/"
    #os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/sdk/obj/lib"
    #os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/sdk/obj/lib/external"
    #os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/hal/obj"
    os.environ["SNORT_LUA_PATH"] = snort_dir + "/lua/"
    os.environ["LUA_PATH"] = snort_dir + "/lua/?.lua;;"
    os.environ["SNORT_DAQ_PATH"] = snort_dir + "/x86_64/lib/"
    os.environ["COVFILE"] = os.path.realpath(bullseye_hal_cov_file)
    if not args.enable_aging:
        os.environ["DISABLE_AGING"] = "1"
    if args.nicmgr_gtest:
        os.environ["DISABLE_NICMGR_HAL_THREAD"] = "1"
    if args.microseg_enable:
       os.environ["MICROSEG_ENABLE"] = "1"

    #hal_dir = nic_dir + "/../bazel-bin/nic/hal"
    os.chdir(nic_dir)

    global hal_log
    global bin_dir
    jsonfile = 'hal.json'
    if args.rtl:
        jsonfile = 'hal_rtl.json'
    if args.gft:
        bin_dir = nic_dir + '/build/x86_64/gft/' + asic + '/bin/'
        jsonfile = 'hal_gft.json'

    hal_log_dir = nic_dir
    if args.test_suf is not None:
        hal_log_dir = nic_dir + "/logs_%s/" % args.test_suf

    os.environ["NON_PERSISTENT_LOG_DIR"] = hal_log_dir
    hal_log = hal_log_dir + "/hal.log"
    os.system("rm -rf %s/hal.log.*" % hal_log_dir)
    device_conf_file = nic_dir + "/conf/device.conf"
    if args.hostpin:
        create_device_conf(device_conf_file, "FORWARDING_MODE_HOSTPIN")
    elif args.classic:
        create_device_conf(device_conf_file, "FORWARDING_MODE_CLASSIC")
    else:
        create_device_conf(device_conf_file, "FORWARDING_MODE_SWITCH")


    '''
    os.system("cp " + nic_dir + "/conf/hal_switch.ini " + nic_dir + "/conf/hal.ini")
    if args.hostpin:
        #jsonfile = 'hal_hostpin.json'
        os.system("cp " + nic_dir + "/conf/hal_hostpin.ini " + nic_dir + "/conf/hal.ini")
    if args.classic:
        #jsonfile = 'hal_classic.json'
        os.system("cp " + nic_dir + "/conf/hal_classic.ini " + nic_dir + "/conf/hal.ini")
    '''

    FNULL = open(os.devnull, 'w')
    FOUT  = open(nic_dir + "/hal_stdout.log", 'w')
    print (bin_dir + '/hal', jsonfile)
    p = Popen([bin_dir+'/hal', "--config", jsonfile], preexec_fn=demote, stdout=FOUT, stderr=FNULL)
    global hal_process
    hal_process = p
    print "* Starting HAL pid (" + str(p.pid) + ")"
    print "- Log file: " + hal_log + "\n"

    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()

    time.sleep(10)
    #return

    #os.system("rm " + nic_dir + "/conf/hal.ini")
    #os.system("rm " + nic_dir + "/conf/device.conf")

    return


#    log.close()

def dump_coverage_data():
    model_process.send_signal(signal.SIGINT)
    if hal_process:
        hal_process.send_signal(signal.SIGUSR1)
    time.sleep(5)


# Run sw/platform's model_server
def run_platform_model_server(args, standalone=False):
    wait_for_hal_and_fte()
    bin_dir = os.path.join(nic_dir, "build/x86_64/iris", asic, "bin")
    lib_dir = os.path.join(nic_dir, "build/x86_64/iris", asic, "lib")
    os.environ["LD_LIBRARY_PATH"] += ":" + lib_dir
    print "LD_LIBRARY_PATH: " + os.environ["LD_LIBRARY_PATH"]
    os.chdir(nic_dir)
    cmd = [os.path.join(bin_dir, 'model_server'), '-d', 'type=accel,bdf=03:00.0']

    global platform_model_server_process
    platform_model_server_process = Popen(cmd)

    print "* Starting platform_model_server pid (" + str(platform_model_server_process.pid) + ")"
    lock = open(lock_file, "a+")
    lock.write(str(platform_model_server_process.pid) + "\n")
    lock.close()

    #wait for exit in standalone mode
    if standalone:
        while not platform_model_server_process.poll():
            continue
        return platform_model_server_process.returncode
    return 0

# Run nicmgr gtest
def run_nicmgr_gtest(args, standalone=False):
    wait_for_hal_and_fte()
    bin_dir = os.path.join(nic_dir, "build/x86_64/iris", asic, "out/nicmgr_gtest")
    lib_dir = os.path.join(nic_dir, "build/x86_64/iris", asic, "lib")
    os.environ["LD_LIBRARY_PATH"] += ":" + lib_dir
    print "LD_LIBRARY_PATH: " + os.environ["LD_LIBRARY_PATH"]
    os.chdir(nic_dir)
    tcs = ""
    if args.testcase:
        tcs = "--gtest_filter="
        result = [x.strip() for x in args.testcase.split(',')]
        for tc in result:
            tcs += "nicmgr_test." + tc + ":"
        tcs = tcs[:-1]

    if args.classic:
        cmd = [os.path.join(bin_dir, 'nicmgr.gtest'), "--classic", tcs]
    else:
        cmd = [os.path.join(bin_dir, 'nicmgr.gtest'), tcs]

    print "Running cmd : ", cmd

    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)


# Run nicmgr
def run_nicmgr(args, standalone=False):
    wait_for_hal_and_fte()
    bin_dir = os.path.join(nic_dir, "build/x86_64/iris", asic, "bin")
    lib_dir = os.path.join(nic_dir, "build/x86_64/iris", asic, "lib")
    os.environ["DOL"] = "1"
    os.environ["LD_LIBRARY_PATH"] += ":" + lib_dir
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf/"
    print "LD_LIBRARY_PATH: " + os.environ["LD_LIBRARY_PATH"]
    os.chdir(nic_dir)
    cmd = [os.path.join(bin_dir, 'nicmgrd'), "-c", "none", "-p", "sim"]

    global nicmgr_process
    nicmgr_process = Popen(cmd)

    print "* Starting nicmgrd pid (" + str(nicmgr_process.pid) + ")"
    lock = open(lock_file, "a+")
    lock.write(str(nicmgr_process.pid) + "\n")
    lock.close()

    #wait for exit in standalone mode
    if standalone:
        while not nicmgr_process.poll():
            continue
        return nicmgr_process.returncode
    return 0

# Run both nicmgr and sw/platform's model_server
def run_nicmgr_platform_model_server(args, standalone=False):
    run_nicmgr(args)
    run_platform_model_server(args)

    #wait for exit in standalone mode
    status = 0
    if standalone:
        nicmgr_done = False
        platform_model_server_done = False
        while True:
            if nicmgr_process and nicmgr_process.poll() is not None:
                nicmgr_done = True
            if platform_model_server_process and platform_model_server_process.poll() is not None:
                platform_model_server_done = True
            if nicmgr_done and platform_model_server_done:
                break

        if nicmgr_process:
            status = nicmgr_process.returncode
        if status == 0 and platform_model_server_process:
            status = platform_model_server_process.returncode
    return 0

# Run Storage DOL
def run_storage_dol(port, args):
    wait_for_hal_and_fte()
    bin_dir = nic_dir + "/../nic/build/x86_64/iris/" + asic + "/bin"
    if args.rtl:
        if args.storage_test:
            cmd = ['./storage_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip), '--test_group', args.storage_test, '--poll_interval', '3600', '--long_poll_interval', '3600']
        else:
            cmd = ['./storage_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip), '--poll_interval', '3600', '--long_poll_interval', '3600']
    else:
        if args.storage_test:
            cmd = ['./storage_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip), '--test_group', args.storage_test]
        else:
            cmd = ['./storage_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip)]
    if args.combined:
        cmd.append('--combined')
    if args.rtl:
        cmd.append('--rtl')
    if args.storage_test and args.storage_test == 'nicmgr':
        cmd.extend(shlex.split('--nicmgr_config_file /sw/platform/src/app/nicmgrd/etc/accel.json'))

    #pass additional arguments to storage_test
    os.chdir(bin_dir)
    if args.storage_runargs:
        cmd.extend(shlex.split(args.storage_runargs))

    print 'Executing command [%s]' % ', '.join(map(str, cmd))
    global stg_dol_process
    if args.combined:
        stg_log = open(stg_dol_log, "w")
        stg_dol_process = Popen(cmd, stdout=stg_log, stderr=stg_log)
        return 0
    else:
        stg_dol_process = Popen(cmd)
        return check_for_completion(None, stg_dol_process, model_process, hal_process, args)

# Run FIPS hal_test
def run_hal_test_fips(port, args):
    wait_for_hal_and_fte()
    bin_dir = nic_dir + "/build/x86_64/iris/" + asic + "/bin"
    script_dir = nic_dir + "/third-party/nist-cavp"
    os.environ["ENGINE_LOG_DIR"] = nic_dir
    cmd = ['./hal_test', '--hal_port', str(port), '--script_dir', script_dir]

    #pass additional arguments to hal_test
    os.chdir(bin_dir)
    if args.hal_test_fips_runargs:
        cmd.extend(shlex.split(args.hal_test_fips_runargs))

    print 'Executing command [%s]' % ', '.join(map(str, cmd))
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run offload test
def run_offload_test(port, args):
    wait_for_hal_and_fte()
    bin_dir = nic_dir + "/../nic/build/x86_64/iris/" + asic + "/bin"
    script_dir = nic_dir + "/third-party/nist-cavp"
    os.environ["ENGINE_LOG_DIR"] = nic_dir
    if args.rtl:
        if args.offload_test:
            cmd = ['./offload_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip), '--test_group', args.offload_test, '--poll_interval', '3600', '--long_poll_interval', '3600']
        else:
            cmd = ['./offload_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip), '--poll_interval', '3600', '--long_poll_interval', '3600']
    else:
        if args.offload_test:
            cmd = ['./offload_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip), '--test_group', args.offload_test]
        else:
            cmd = ['./offload_test', '--hal_port', str(port), '--hal_ip', str(args.hal_ip)]
    if args.rtl:
        cmd.append('--rtl')
    if args.offload_test and args.offload_test == 'nicmgr':
        cmd.extend(shlex.split('--nicmgr_config_file /sw/platform/src/app/nicmgrd/etc/offload.json'))

    #pass additional arguments to offload_test
    os.chdir(bin_dir)
    if args.offload_runargs:
        cmd.extend(shlex.split(args.offload_runargs))

    print 'Executing command [%s]' % ', '.join(map(str, cmd))
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run athena app
def run_athena_app(args):
    bin_dir = nic_dir + "/../nic/build/x86_64/athena/" + asic + "/bin"
    os.environ["LOG_DIR"] = nic_dir + '/'
    os.environ["PERSISTENT_LOG_DIR"] = nic_dir + '/'
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    os.environ["PDSPKG_TOPDIR"] = nic_dir

    if args.disable_hugepages is False:
        #Huge-pages for DPDK
        os.system("echo 2048 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages")
        os.system("mkdir -p /dev/hugepages")
        os.system("mount -t hugetlbfs nodev /dev/hugepages")

    os.chdir(nic_dir)
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/athena/pipeline.json", nic_dir + "/conf/pipeline.json")

    cmd = ['./athena_app', '-c', 'hal.json']

    #pass additional arguments to athena_app, if any
    os.chdir(bin_dir)
    if args.athena_app_runargs:
        cmd.extend(shlex.split(args.athena_app_runargs))

    print 'Executing command [%s]' % ', '.join(map(str, cmd))
    p = Popen(cmd)
    print "* Starting athena_app: pid (" + str(p.pid) + ")"

    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()

    time.sleep(10)
    return check_for_completion(p, None, model_process, None, args)

# Run athena gtests
def run_athena_gtests(args):
    bin_dir = nic_dir + "/../nic/build/x86_64/athena/" + asic + "/bin"
    os.environ["LOG_DIR"] = nic_dir + '/'
    os.environ["PERSISTENT_LOG_DIR"] = nic_dir + '/'
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    os.environ["PDSPKG_TOPDIR"] = nic_dir

    #Huge-pages for DPDK
    os.system("echo 2048 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages")
    os.system("mkdir -p /dev/hugepages")
    os.system("mount -t hugetlbfs nodev /dev/hugepages")

    os.chdir(nic_dir)
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/athena/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ['./athena_gtests', '-c', 'hal.json']
    os.chdir(bin_dir)

    print 'Executing command [%s]' % ', '.join(map(str, cmd))
    p = Popen(cmd)
    print "* Starting athena_gtests: pid (" + str(p.pid) + ")"

    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()

    time.sleep(10)
    return check_for_completion(p, None, model_process, None, args)

# Run filter tests for libiris-c.so
def run_filter_gtest(args):
    #os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    #os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/../bazel-bin/nic/model_sim/"
    os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/../platform/gen/x86_64/lib/"
    os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/build/x86_64/iris/" + asic + "/lib/"
    os.chdir(nic_dir)
    hal_api_gtest_bin_dir = nic_dir + "/../platform/gen/x86_64/src/lib/hal_api/gtest/filter_test"
    hal_api_gtest_bin_dir1 = nic_dir + "/../platform/gen/x86_64/src/lib/hal_api/gtest/filter_smart_test"
    if args.classic is True:
        cmd = [hal_api_gtest_bin_dir + '/hal_api_filter_test']
    else:
        cmd = [hal_api_gtest_bin_dir1 + '/hal_api_filter_smart_test']
    p = Popen(cmd)
    #p.communicate()
    return check_for_completion(p, None, model_process, hal_process, args)

# Run span tests for libiris-c.so
def run_span_gtest(args):
    #os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    #os.environ["LD_LIBRARY_PATH"] += ":" + nic_dir + "/../bazel-bin/nic/model_sim/"
    os.chdir(nic_dir)
    cmd = ['../bazel-bin/nic/hal/iris-c/gtest/span_test']
    p = Popen(cmd)
    #p.communicate()
    return check_for_completion(p, None, model_process, hal_process, args)

# Run GFT tests
def run_gft_test(args):
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    os.chdir(nic_dir)
    cmd = ['build/x86_64/gft/' + asic + '/bin/gft_test']
    p = Popen(cmd)
    #p.communicate()
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Apollo tests
def run_apollo_test(args):
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.environ["PDSPKG_TOPDIR"] = nic_dir
    os.chdir(nic_dir)
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/apollo/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ['build/x86_64/apollo/' + asic + '/bin/apollo_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Artemis Scale tests
def run_artemis_scale_test(args):
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.environ["VPP_IPC_MOCK_MODE"] = "1"
    os.environ["PDSPKG_TOPDIR"] = nic_dir
    os.chdir(nic_dir)
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/artemis/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ["build/x86_64/artemis/" + asic + "/bin/artemis_scale_test",
           '-c', "hal.json",
           '-i', "apollo/test/scale/artemis/scale_cfg_sim.json",
           "--gtest_output=", "xml:build/x86_64/artemis/" + asic + "/gtest_results/artemis_scale_test.xml"]
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Apollo Scale tests
def run_apollo_scale_test(args):
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.environ["SKIP_VERIFY"] = "1"
    os.environ["VPP_IPC_MOCK_MODE"] = "1"
    os.environ["PDSPKG_TOPDIR"] = nic_dir
    os.chdir(nic_dir)
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/apollo/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ["build/x86_64/apollo/" + asic + "/bin/apollo_scale_test",
           '-c', "hal.json",
           '-i', "apollo/test/scale/scale_cfg_sim.json",
           "--gtest_output=", "xml:build/x86_64/apollo/" + asic + "/gtest_results/apollo_scale_test.xml"]
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Apollo VXLAN Scale tests
def run_apollo_vxlan_scale_test(args):
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.environ["SKIP_VERIFY"] = "1"
    os.environ["APOLLO_TEST_TEP_ENCAP"] = "vxlan"
    os.environ["VPP_IPC_MOCK_MODE"] = "1"
    os.environ["PDSPKG_TOPDIR"] = nic_dir
    os.chdir(nic_dir)
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/apollo/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ["build/x86_64/apollo/" + asic + "/bin/apollo_scale_test",
           '-c', "hal.json",
           '-i', "apollo/test/scale/scale_cfg_sim.json",
           "--gtest_output=", "xml:build/x86_64/apollo/" + asic + "/gtest_results/apollo_scale_test.xml"]
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Artemis tests
def run_artemis_test(args):
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.chdir(nic_dir)
    os.environ["PDSPKG_TOPDIR"] = nic_dir
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/artemis/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ['build/x86_64/artemis/' + asic + '/bin/artemis_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Apulu tests
def run_apulu_test(args):
    os.environ["CONFIG_PATH"] = nic_dir + "/conf/"
    os.chdir(nic_dir)
    os.environ["PDSPKG_TOPDIR"] = nic_dir
    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass
    os.symlink(nic_dir + "/conf/apulu/pipeline.json", nic_dir + "/conf/pipeline.json")
    cmd = ['build/x86_64/apulu/' + asic + '/bin/apulu_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run L2Switch tests
def run_l2switch_test(args):
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf/"
    os.chdir(nic_dir)
    cmd = ['build/x86_64/l2switch/' + asic + '/bin/l2switch_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Elektra tests
def run_elektra_test(args):
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf/"
    os.chdir(nic_dir)
    cmd = ['build/x86_64/elektra/' + asic + '/bin/elektra_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run Phoebus tests
def run_phoebus_test(args):
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf/"
    os.chdir(nic_dir)
    cmd = ['build/x86_64/phoebus/' + asic + '/bin/phoebus_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# Run gft16 tests
def run_gft16_test(args):
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf/"
    os.chdir(nic_dir)
    cmd = ['build/x86_64/gft16/' + asic + '/bin/gft16_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)


# Run Hello tests
def run_hello_test(args):
    os.environ["HAL_CONFIG_PATH"] = nic_dir + "/conf"
    os.chdir(nic_dir)
    cmd = ['build/x86_64/hello/' + asic + '/bin/hello_test']
    p = Popen(cmd)
    return check_for_completion(p, None, model_process, hal_process, args)

# DOL
def check_for_completion(nw_dol_process, stg_dol_process,  model_process, hal_process, args):
    nw_dol_done = False
    stg_dol_done = False
    nicmgr_done = False
    if nw_dol_process is None:
        nw_dol_done = True
    if stg_dol_process is None:
        stg_dol_done = True
    while 1:
        if nw_dol_process and nw_dol_process.poll() is not None:
            nw_dol_done = True
        if stg_dol_process and stg_dol_process.poll() is not None:
            stg_dol_done = True
        if nw_dol_done and stg_dol_done:
            break
        if model_process and model_process.poll() is not None:
            break
        if hal_process and hal_process.poll() is not None:
            break
        time.sleep(5)

    status = 0
    if nw_dol_process:
        status = nw_dol_process.returncode
    if status == 0 and stg_dol_process:
        status = stg_dol_process.returncode
    if args.rtl and status == 0:
        # Wait for runtest to finish only in case of DOL finishing
        # successfully.
        count = 0
        while model_process and model_process.poll() is None:
            count += 5
            time.sleep(5)
            if count % 300 == 0:
                print "RUNTEST/SIMV not exited after %d seconds" % count

            exit_timeout = 4
            if 'MODEL_EXIT_TIMEOUT' in os.environ:
                exit_timeout = os.environ['MODEL_EXIT_TIMEOUT']
            if count == exit_timeout * 60 * 60:
                print "%s" % '='*80
                print "             %d hour Exit Timeout Reached. Killing all processes." % exit_timeout
                print "%s" % '='*80
                return 1


    exitcode = 0
    check_for_cores = False;
    if model_process:
        print "* MODEL exit code " + str(model_process.returncode)
        if model_process.returncode:
            exitcode = model_process.returncode
            core_file = model_core_path + "/core." + str(model_process.pid)
            process_model_core(core_file)

    if hal_process:
        print "* HAL exit code " + str(hal_process.returncode)
        if hal_process.returncode:
            exitcode = hal_process.returncode
            core_file = hal_core_path + "/core." + str(hal_process.pid)
            process_hal_core(core_file)

    if nw_dol_process:
        print "* NW DOL exit code " + str(nw_dol_process.returncode)
        if nw_dol_process.returncode:
            exitcode = nw_dol_process.returncode

    if stg_dol_process:
        print "* Storage DOL exit code " + str(stg_dol_process.returncode)
        if stg_dol_process.returncode:
            exitcode = stg_dol_process.returncode

    return exitcode


def run_dol(args):
    dol_dir = nic_dir + "/../dol"
    os.chdir(dol_dir)

    #log = open(dol_log, "w")
    cmd = ['./main.py', '--topo', args.topology, '--pipeline', args.pipeline ]
    if args.modlist is not None:
        cmd.append('--modlist')
        cmd.append(args.modlist)
    if args.feature is not None:
        cmd.append('--feature')
        cmd.append(args.feature)
    if args.sub is not None:
        cmd.append('--sub')
        cmd.append(args.sub)
    if args.test is not None:
        cmd.append('--test')
        cmd.append(args.test)
    if args.cfgjson is not None:
        cmd.append('--cfgjson')
        cmd.append(args.cfgjson)
    if args.testcase is not None:
        cmd.append('--testcase')
        cmd.append(args.testcase)
    if args.hostpin is True:
        cmd.append('--hostpin')
    if args.classic is True:
        cmd.append('--classic')
    if args.coveragerun is True:
        cmd.append('--coveragerun')
    if args.regression is True:
        cmd.append('--regression')
    if args.skipverify is True:
        cmd.append('--skipverify')
    if args.rtl and args.skipverify:
        cmd.append('--eth_mode=ionic')
    if args.tcscale is not None:
        cmd.append('--tcscale')
        cmd.append(args.tcscale)
    if args.modscale is not None:
        cmd.append('--modscale')
        cmd.append(args.modscale)
    if args.dryrun is True:
        cmd.append('--dryrun')
    if args.configonly is True:
        cmd.append('--config-only')
    if args.shuffle:
        cmd.append('--shuffle')
    if args.mbt is True:
        cmd.append('--mbt')
    if args.rtl is True:
        cmd.append('--rtl')
    if args.latency is True:
        cmd.append('--latency')
    if args.pps is True:
        cmd.append('--pps')
    if args.lite is True:
        cmd.append('--lite')
    if args.l2l is True:
        cmd.append('--l2l')
    if args.gft is True:
        cmd.append('--gft')
    if args.perf is True:
        cmd.append('--perf')
    if args.e2e_mode == "dol-auto":
        cmd.append("--e2e")
    if args.debug:
        cmd.append("--debug")
    if args.verbose:
        cmd.append("--verbose")
    if args.agent:
        cmd.append("--agent")
    if args.naplescontainer:
        cmd.append("--naplescontainer")

    global nw_dol_process
    if args.coveragerun:
        #Increasing timeout for coverage runs only.
        dol_env = os.environ.copy()
        dol_env["MODEL_TIMEOUT"] = "300"
        nw_dol_process = Popen(cmd, env=dol_env)
    else:
        nw_dol_process = Popen(cmd)
    print "* Starting DOL pid (" + str(nw_dol_process.pid) + ")"
    print "- Log file: " + dol_log + "\n"

    lock = open(lock_file, "a+")
    lock.write(str(nw_dol_process.pid) + "\n")
    lock.close()
    #p.communicate()
    #log.close()

    if args.combined:
        return 0
    else:
        return check_for_completion(nw_dol_process, None, model_process, hal_process, args)

def run_mbt(args, standalone=True):
    mbt_dir = nic_dir + "/../mbt"
    os.chdir(mbt_dir)

    log = open(mbt_log, "a+")
    cmd = ['./main.py']
    # If topology is set, then it is being run in DOL mode.
    if not standalone:
        cmd.append('--mbt')
    if args.mbtrandomseed:
        cmd.append('--mbtrandomseed')
        cmd.append(args.mbtrandomseed)
    p = Popen(cmd)
    print "* Starting Model based testing with  pid (" + str(p.pid) + ")"
    print "- Log file: " + mbt_log + "\n"

    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()

    status = None
    if standalone:
        p.communicate()
        status = p.returncode
    return status

def find_port():
    s = socket.socket()
    s.bind(('', 0))
    port = s.getsockname()[1]
    s.close()

    return port

def is_running(pid):
    try:
        os.kill(pid, 0)
        return True
    except OSError:
        return False

def cleanup(keep_logs=True):
    print_naples_container_cores()
    bringdown_naples_container()
    print "* Killing running processes:"

    if not os.path.exists(lock_file):
        return
    lock = open(lock_file, "r")
    pidlist = lock.readlines()
    pidlist.reverse()
    for pid in pidlist:
        if is_running(int(pid)):
            print "Sending SIGTERM to process %d" % int(pid)
            try:
                os.kill(int(pid), signal.SIGTERM)
            except:
                print "No process found with PID %d" % int(pid)

            # wait for the process to die
            attempts = 3
            while is_running(int(pid)) and (attempts > 0):
                print "Waiting for process %d to die" % int(pid)
                time.sleep(1)
                os.waitpid(int(pid), os.WNOHANG);
                attempts -= 1

    print "Sending SIGTERM to run.py PG %d" % os.getpid()
    #try:
    #    os.killpg(os.getpid(), signal.SIGTERM)
    #except:
    #    print "No processes found with PGID %d" % os.getpid()

    lock.close()
    os.remove(lock_file)

    # print "\n* Removing log files:"

    if keep_logs:
        return

    for log in build_log, model_log, hal_log, dol_log:
        if os.path.isfile(log):
            print "- " + log
            os.remove(log)
        else:
            print "- " + log + " - not found"

def run_v6_e2e_tlsproxy_dol():
    os.chdir(nic_dir)
    cmd = ['./tools/run_v6_e2e_tlsproxy_test.py']
    p = Popen(cmd)
    print "* Starting E2E TLS Proxy DOL, pid (" + str(p.pid) + ")"
    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()
    p.communicate()
    print("* FAIL:" if p.returncode != 0 else "* PASS:") + " E2E TLS Proxy DOL, exit code ", p.returncode
    return p.returncode

def run_e2e_tlsproxy_dol():
    os.chdir(nic_dir)
    cmd = ['./tools/run_e2e_tlsproxy_test.py']
    p = Popen(cmd)
    print "* Starting E2E TLS Proxy DOL, pid (" + str(p.pid) + ")"
    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()
    p.communicate()
    print("* FAIL:" if p.returncode != 0 else "* PASS:") + " E2E TLS Proxy DOL, exit code ", p.returncode
    return p.returncode

def run_e2e_l7_dol():
    os.chdir(nic_dir)
    cmd = ['./tools/run_e2e_l7_test.py']
    p = Popen(cmd)
    print "* Starting E2E L7 DOL, pid (" + str(p.pid) + ")"
    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()
    p.communicate()
    print("* FAIL:" if p.returncode != 0 else "* PASS:") + " E2E L7 DOL, exit code ", p.returncode
    return p.returncode
# main()

def run_e2e_infra_dol(mode, e2espec = None, naplescontainer = None):
    os.chdir(nic_dir)
    cmd = ['go', 'run', './e2etests/go/e2e_main.go']
    if e2espec:
        cmd.extend(["--e2e-spec="+e2espec])
    if naplescontainer:
        cmd.extend(["-naples-container="+naplescontainer])
    p = Popen(cmd)
    print "* Starting E2E , pid (" + str(p.pid) + ")"
    lock = open(lock_file, "a+")
    lock.write(str(p.pid) + "\n")
    lock.close()
    p.communicate()
    print("* FAIL:" if p.returncode != 0 else "* PASS:") + " E2E ,DOL, exit code ", p.returncode
    return p.returncode

def wait_for_line_log(log_file, line_match, timeout=None):
    log2 = open(log_file, "r")
    t_end = time.time() + timeout if timeout else None
    while 1:
        for line in log2.readlines():
            if line_match in line.strip():
                log2.close()
                return
        log2.seek(0)
        if t_end and time.time() > t_end:
            raise Exception("Timeout Waiting for line :%s in log : %s" %(line_match, log_file))

def bringup_naples_container(args):
    bringdown_naples_container()
    hal_log_file = os.environ['HOME'] + "/naples/data/logs/hal.log"
    naples_sim_log_file =  os.environ['HOME'] + "/naples/data/logs/start-naples.log"
    agent_log_file = os.environ['HOME'] + "/naples/data/logs/agent.log"

    def get_hal_port(log_file):
        log2 = open(log_file, "r")
        loop = 1
        while loop == 1:
            for line in log2.readlines():
                if "listening on" in line:
                    return line.split(":")[-1].strip()
        log2.close()
        return


    def wait_for_naples_sim_to_be_up():
        wait_for_line_log(naples_sim_log_file, "NAPLES services/processes up and running",
                timeout=naples_sim_timeout)

    def wait_for_agent_to_be_up():
        wait_for_line_log(agent_log_file,
         "Starting server at", timeout=naples_agent_timeout)

    def wait_for_hal_to_be_up():
        wait_for_line_log(hal_log_file,
         "listening on", timeout=naples_hal_timeout)
        wait_for_line_log(hal_log_file,
         "Starting FTE instance", timeout=naples_hal_timeout)

    for file in [hal_log_file, naples_sim_log_file, agent_log_file]:
        if os.path.isfile(file):
            open(file, 'w').close()

    os.chdir(naples_container_image_dir)
    if not os.path.isfile(naples_container_image):
        print ("Nic Container image %s was not found", naples_container_image)
        sys.exit(1)
    #Extract the container image
    print ("Extracting Container image")
    retcode = call(['tar', '-xzvf', naples_container_image])
    if retcode:
        print ("Extraction failed!")
        sys.exit(1)

    if args.qemu:
        retcode = call([naples_container_startup_script, '--qemu'])
    else:
        retcode = call(naples_container_startup_script)

    if retcode:
        print ("Bringing up Nic container failed")
        sys.exit(1)
    os.chdir(nic_dir)
    time.sleep(5)
    print ("Waiting for HAL to be up..")
    wait_for_hal_to_be_up()
    os.environ["HAL_GRPC_PORT"] = get_hal_port(hal_log_file)
    print ("Wating for naples sim to be up")
    wait_for_naples_sim_to_be_up()
    print ("Wating for agent server to be up")
    time.sleep(5)
    wait_for_agent_to_be_up()
    print ("Nic container bring up was successfull.")


def bringdown_naples_container():
    print ("Bringing down nic container")
    try:
        os.chdir(naples_container_image_dir)
        retcode = call(["docker", "stop", naples_container_name])
        print ("Stopped nic container")
        retcode = call(["docker", "rmi", "-f", naples_container_image_name])
        print ("Removed nic container image")
    except:
        retcode = 0
    if retcode:
        print("Bringdown of Nic Container failed")
    os.chdir(nic_dir)

def print_naples_container_cores():
    print_core_script = "/naples/nic/tools/print-cores.sh"
    try:
        retcode = call(["docker", "exec", "-it", naples_container_name, print_core_script])
    except:
        retcode = 0
    if retcode:
        print("Printing container cores failed.")

def run_dol_test(args):
    if args.gft:
        wait_for_hal()
    else:
        wait_for_hal_and_fte()
    status = run_dol(args)
    if status == 0:
        if (args.e2etls):
            status = run_e2e_tlsproxy_dol()
        elif (args.e2el7):
            status = run_e2e_l7_dol()
        elif (args.v6e2etls):
                status = run_v6_e2e_tlsproxy_dol()
        elif (args.e2e_mode and args.e2e_mode != "dol-auto"):
            status = run_e2e_infra_dol(args.e2e_mode, args.e2e_spec,
                                        naplescontainer = naples_container_name if args.naplescontainer else None)
    return status

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--build", action="store_true", help="run build")
    parser.add_argument("-c", "--cleanup", action="store_true",
                        help="cleanup running process")
    parser.add_argument("--naplescontainer", action="store_true",
                        help="run with Nic container image")
    parser.add_argument("--qemu", action="store_true",
                        help="run with Qemu as host")
    parser.add_argument("--modellogs", action="store_true",
                        help="run with model logs enabled")
    parser.add_argument("--coveragerun", action="store_true",
                        help="run with model logs enabled")
    parser.add_argument('--topo', dest='topology', default='base',
                        help='Run for a specific topology')
    parser.add_argument('--pipeline', dest='pipeline', default='iris',
                        help='Run for a specific pipeline')
    parser.add_argument('--modlist', dest='modlist',
                        default=None, help='Module List File')
    parser.add_argument('--feature', dest='feature',
                        default=None, help='Run all tests of a feature.')
    parser.add_argument('--sub', dest='sub',
                        default=None, help='Run all tests of a sub-feature.')
    parser.add_argument('--test', dest='test',
                        default=None, help='Module List File')
    parser.add_argument('--testcase', dest='testcase',
                        default=None, help='Module List File')
    parser.add_argument('--hostpin', dest='hostpin', action="store_true",
                        help='Run tests in Hostpinned mode.')
    parser.add_argument('--classic', dest='classic', action="store_true",
                        help='Run tests in Classic NIC mode.')
    parser.add_argument('--storage', dest='storage', action="store_true",
                        help='Run storage dol as well.')
    parser.add_argument('--hal_test_fips', dest='hal_test_fips', action="store_true",
                        help='Run FIPS hal_test.')
    parser.add_argument('--offload', dest='offload', action="store_true",
                        help='Run offload dol as well.')
    parser.add_argument('--athena_app', dest='athena_app', action="store_true",
                        help='Run athena_app dol as well.')
    parser.add_argument('--athena_gtests', dest='athena_gtests', action="store_true",
                        help='Run athena_gtests as well.')
    parser.add_argument('--nicmgr', dest='nicmgr', action="store_true",
                        help='Run nicmgr standalone.')
    parser.add_argument('--nicmgr_platform_model_server', dest='nicmgr_platform_model_server', action="store_true",
                        help='Run both nicmgr and platform model_server.')
    parser.add_argument('--rtl', dest='rtl', action="store_true",
                        help='Run RTL sim as well.')
    parser.add_argument('--noverilog', dest='noverilog', action="store_true",
                        help='Skip verilog compile dol as well.')
    parser.add_argument('--runtest_runargs', dest='runtest_runargs', default='',
                        help='any extra options that should be passed to runtest as run_args.')
    parser.add_argument('--model_test', dest='model_test', default=None,
                        help='Model test name')
    # This is not needed anymore. It will be automatically derived from skipverify flag.
    #parser.add_argument('--one_pkt_mode', dest='one_pkt_mode', action="store_true",
    #                    help='Enable DOL one pkt mode')
    parser.add_argument('--no_asic_dump', dest='no_asic_dump', action="store_true",
                        help='Disable rtl dump')
    parser.add_argument('--test_suf', dest='test_suf', default=None,
                        help='Model test suffix.')
    parser.add_argument("--asmcov", action="store_true",
                        help="Generate ASM coverage for this run")
    parser.add_argument('--regression', dest='regression',
                        action='store_true', help='Run tests in regression mode.')
    parser.add_argument('--skipverify', dest='skipverify',
                        action='store_true', help='Skip Verification all tests.')
    parser.add_argument('--tcscale', dest='tcscale', default=None,
                        help='Testcase Scale Factor.')
    parser.add_argument('--modscale', dest='modscale', default=None,
                        help='Module Scale Factor.')
#    parser.add_argument('--cfgscale', dest='cfgscale', default=None,
#                        help='Configuration Scale Factor.')
    parser.add_argument('--dryrun', dest='dryrun', action='store_true',
                        help='Dry-Run mode. (No communication with HAL & Model)')
    parser.add_argument("--configtest", action="store_true",
                        help="Run Config tests.")
    parser.add_argument("--config-only", dest='configonly', action="store_true",
                        help="Generate ASM coverage for this run")
    parser.add_argument("--v6-e2e-tls-dol", dest='v6e2etls', action="store_true",
                        default=None, help="Run V6 E2E TLS DOL")
    parser.add_argument("--e2e-tls-dol", dest='e2etls', action="store_true",
                        default=None, help="Run E2E TLS DOL")
    parser.add_argument("--e2e-l7-dol", dest='e2el7', action="store_true",
                        default=None, help="Run E2E L7 DOL")
    parser.add_argument('--e2e-mode', dest='e2e_mode', default=None,
                    choices=["dol-auto", "auto", "manual"], help='E2E Test mode.')
    parser.add_argument('--e2e-spec', dest='e2e_spec', default=None,
                        help='E2E Spec to bring up if manual mode.')
    parser.add_argument("--gft", dest='gft', action="store_true",
                        default=False, help="GFT tests")
    parser.add_argument("--gft_gtest", dest='gft_gtest', action="store_true",
                        default=False, help="Run GFT gtests")
    parser.add_argument("--filter_gtest", dest='filter_gtest', action="store_true",
                        default=False, help="Run filter gtests")
    parser.add_argument("--span_gtest", dest='span_gtest', action="store_true",
                        default=False, help="Run Span gtests")
    parser.add_argument("--nicmgr_gtest", dest='nicmgr_gtest', action="store_true",
                        default=False, help="Run nicmgr gtests")
    parser.add_argument("--apollo_gtest", dest='apollo_gtest', action="store_true",
                        default=False, help="Run Apollo2 gtests")
    parser.add_argument("--artemis_gtest", dest='artemis_gtest', action="store_true",
                        default=False, help="Run Artemis gtests")
    parser.add_argument("--apulu_gtest", dest='apulu_gtest', action="store_true",
                        default=False, help="Run Apulu gtests")
    parser.add_argument("--apollo_scale_test", dest='apollo_scale_test', action="store_true",
                        default=False, help="Run Apollo scale tests")
    parser.add_argument("--artemis_scale_test", dest='artemis_scale_test', action="store_true",
                        default=False, help="Run Artemis scale tests")
    parser.add_argument("--apollo_scale_vxlan_test", dest='apollo_scale_vxlan_test', action="store_true",
                        default=False, help="Run Apollo scale vxlan tests")
    parser.add_argument("--l2switch_gtest", dest='l2switch_gtest', action="store_true",
                        default=False, help="Run L2Switch gtests")
    parser.add_argument("--elektra_gtest", dest='elektra_gtest', action="store_true",
                        default=False, help="Run Elektra gtests")
    parser.add_argument("--phoebus_gtest", dest='phoebus_gtest', action="store_true",
                        default=False, help="Run Phoebus gtests")
    parser.add_argument("--gft16_gtest", dest='gft16_gtest', action="store_true",
                        default=False, help="Run gft16 gtests")
    parser.add_argument("--hello_gtest", dest='hello_gtest', action="store_true",
                        default=False, help="Run Apollo2 gtests")
    parser.add_argument('--shuffle', dest='shuffle', action="store_true",
                        help='Shuffle tests and loop for X times.')
    parser.add_argument('--mbt', dest='mbt', default=None,
                        action='store_true',
                        help='Modify DOL config testre starting packet tests')
    parser.add_argument('--mbt_test', dest='mbt_test', default=None,
                        action='store_true',
                        help='Modify DOL config testre starting packet tests')
    parser.add_argument('--storage_test', dest='storage_test', default=None,
                        help='Run only a subtest of storage test suite')
    parser.add_argument('--storage_runargs', dest='storage_runargs', default='',
                        help='any extra options that should be passed to storage as run_args.')
    parser.add_argument('--hal_test_fips_runargs', dest='hal_test_fips_runargs', default='',
                        help='FIPS hal_test command arguments.')
    parser.add_argument('--offload_test', dest='offload_test', default=None,
                        help='Run only a subtest of offload test suite')
    parser.add_argument('--offload_runargs', dest='offload_runargs', default='',
                        help='any extra options that should be passed to offload as run_args.')
    parser.add_argument('--disable_hugepages', dest='disable_hugepages', action='store_true',
                        help='Disable hugepages setup')
    parser.add_argument('--athena_app_runargs', dest='athena_app_runargs', default='',
                        help='any extra options that should be passed to athena_app as run_args.')
    parser.add_argument('--no_error_check', dest='no_error_check', default=None,
                        action='store_true',
                        help='Disable model error checking')
    parser.add_argument('--mbtrandomseed', dest='mbtrandomseed', default=None,
                        help='Seed for random numbers used in model based tests')
    parser.add_argument('--latency', dest='latency', default=None,
                        action='store_true', help='Latency test.')
    parser.add_argument('--pps', dest='pps', default=None,
                        action='store_true', help='PPS Test.')
    parser.add_argument('--lite', dest='lite', default=None,
                        action='store_true', help='Lite Sanity Test.')
    parser.add_argument('--l2l', dest='l2l', default=None,
                        action='store_true', help='Local-2-Local Endpoint RDMA Sanity Run.')
    parser.add_argument('--port_mode', dest='port_mode', default='nomac',
                        help='In RTL mode choose how many ports are active: (nomac/8x25/2x100)')
    parser.add_argument('--perf', dest='perf', default=None,
                        action='store_true', help='Run Perf tests.')
    parser.add_argument('--cfgjson', dest='cfgjson', default=None,
                        help='Dump DOL configuration to json file')
    parser.add_argument('--zmqtcp', dest='zmqtcp', default=None,
                        action='store_true', help='Run for ZEBU environment.')
    parser.add_argument('--hal_ip', dest='hal_ip', default='localhost',
                        help='HAL IP Address.')
    parser.add_argument('--model_ip', dest='model_ip', default=None,
                        help='Model IP Address.')
    parser.add_argument('--debug', dest='debug', default=None,
                        action='store_true', help='Set loglevel to DEBUG.')
    parser.add_argument('--verbose', dest='verbose', default=None,
                        action='store_true', help='Set loglevel to VERBOSE.')
    parser.add_argument('--rtl_coverage', dest='rtl_coverage', default=None, action='store_true',
                        help='Dump RTL toggle coverage')
    parser.add_argument('--agent', dest='agent', default=None, action='store_true',
                        help='Run with agent.')
    parser.add_argument('--combined', dest='combined', default=False, action='store_true',
                        help='Run storage and network tests combined')
    parser.add_argument("-n", "--nocleanup", action="store_true",
                        help="Skip cleanup during shut down")
    parser.add_argument("--enable-aging", dest='enable_aging', action="store_true",
                        help="Enable aging in HAL")
    parser.add_argument("--microseg-enable", dest='microseg_enable', action="store_true",
                        help="Enable microseg mode in HAL")

    args = parser.parse_args()

    if not args.dryrun and args.rtl == False and (args.latency or args.pps):
        print "ERROR: latency or pps option cannot be used for non RTL runs."
        sys.exit(1)

    zmq_soc_dir = nic_dir
    if args.test_suf is not None:
        os.system("mkdir -p " + nic_dir + "/logs_%s" % args.test_suf)
        #os.environ['MODEL_SOCKET_NAME'] = "zmqsock_%s" % args.test_suf
        global lock_file
        lock_file = nic_dir + "/logs_%s/.run.pid" % args.test_suf
        zmq_soc_dir = nic_dir + "/logs_%s/" % args.test_suf
    os.environ['ZMQ_SOC_DIR'] = zmq_soc_dir
    os.environ['MODEL_SHMKEY_PATH'] = nic_dir
    os.environ['MODEL_SHMKEY_PROJID'] = '1'
    if args.rtl:
        os.environ['HAL_PLATFORM_MODE_RTL'] = '1'

    # Hack: Since GFT and IRIS are combined in DOLs
    if args.pipeline == 'gft':
        args.pipeline = 'iris'

    if args.cleanup:
        if os.path.isfile(lock_file):
            cleanup(keep_logs=False)
        else:
            print "Nothing to cleanup"

        sys.exit(0)

    if args.build:
        r = build()
    else:
        r = 0

    if r != 0:
        print "- build failed!"
        sys.exit(1)

    port = find_port()
    if args.mbt_test:
        port = int(os.environ["HAL_GRPC_PORT"])
    print "* Using port (" + str(port) + ") for HAL\n"
    os.environ["HAL_GRPC_PORT"] = str(port)

    if args.zmqtcp:
        os.environ['MODEL_ZMQ_TYPE_TCP'] = '1'
        zmq_tcp_port = find_port()
        os.environ['MODEL_ZMQ_TCP_PORT'] = str(zmq_tcp_port)

    if args.dryrun is False:
        if args.naplescontainer:
            bringup_naples_container(args)
        else:
            if args.rtl:
                run_rtl(args)
            else:
                run_model(args)
            if args.gft_gtest is False and \
                args.apollo_gtest is False and \
                args.artemis_gtest is False and \
                args.apulu_gtest is False and \
                args.artemis_scale_test is False and \
                args.apollo_scale_test is False and \
                args.apollo_scale_vxlan_test is False and \
                args.athena_app is False and \
                args.athena_gtests is False and \
                args.l2switch_gtest is False and \
                args.elektra_gtest is False and \
                args.gft16_gtest is False and \
                args.phoebus_gtest is False and \
                args.hello_gtest is False:
                run_hal(args)

    if args.storage and args.feature not in [None, 'storage'] and args.combined is False:
        print "ERROR: Use --combined to run storage and networking tests together\n"
        print "Feature is " + str(args.feature)
        sys.exit(1)

    if args.storage and args.combined is False:
        status = run_storage_dol(port, args)
        if status != 0:
            print "- Storage dol failed, status=", status
    elif args.filter_gtest:
        status = run_filter_gtest(args)
        if status != 0:
            print "- Filter test failed, status=", status
    elif args.nicmgr_gtest:
        status = run_nicmgr_gtest(args)
        if status != 0:
            print "- nicmgr gtest failed, status=", status
    elif args.span_gtest:
        status = run_span_gtest(args)
        if status != 0:
            print "- Span test failed, status=", status
    elif args.gft_gtest:
        status = run_gft_test(args)
        if status != 0:
            print "- GFT test failed, status=", status
    elif args.apollo_gtest:
        status = run_apollo_test(args)
        if status != 0:
            print "- Apollo2 test failed, status=", status
    elif args.artemis_gtest:
        status = run_artemis_test(args)
        if status != 0:
            print "- Artemis test failed, status=", status
    elif args.athena_app:
        status = run_athena_app(args)
        if status != 0:
            print "- Athena test failed, status=", status
    elif args.athena_gtests:
        status = run_athena_gtests(args)
        if status != 0:
            print "- Athena gtests failed, status=", status
    elif args.apulu_gtest:
        status = run_apulu_test(args)
        if status != 0:
            print "- Apulu test failed, status=", status
    elif args.artemis_scale_test:
        status = run_artemis_scale_test(args)
        if status != 0:
            print "- Artemis scale test failed, status=", status
    elif args.apollo_scale_test:
        status = run_apollo_scale_test(args)
        if status != 0:
            print "- Apollo scale test failed, status=", status
    elif args.apollo_scale_vxlan_test:
        status = run_apollo_vxlan_scale_test(args)
        if status != 0:
            print "- Apollo scale VXLAN test failed, status=", status
    elif args.l2switch_gtest:
        status = run_l2switch_test(args)
        if status != 0:
            print "- L2Switch test failed, status=", status
    elif args.elektra_gtest:
        status = run_elektra_test(args)
        if status != 0:
            print "- Elektra test failed, status=", status
    elif args.phoebus_gtest:
        status = run_phoebus_test(args)
        if status != 0:
            print "- Phoebus test failed, status=", status
    elif args.gft16_gtest:
        status = run_gft16_test(args)
        if status != 0:
            print "- gft16 test failed, status=", status
    elif args.hello_gtest:
        status = run_hello_test(args)
        if status != 0:
            print "- Hello test failed, status=", status
    elif args.mbt and not args.feature:
        status = run_mbt(args)
        if status != 0:
            print "- MBT test failed, status=", status
    elif args.mbt_test:
        mbt_port = int(os.environ["MBT_GRPC_PORT"])
        print "* Using port (" + str(mbt_port) + ") for mbt\n"
        status = run_dol_test(args)
    elif (args.e2e_mode and args.e2e_mode != "dol-auto"):
            status = run_e2e_infra_dol(args.e2e_mode, args.e2e_spec,
                                        naplescontainer = naples_container_name if args.naplescontainer else None)
    elif args.nicmgr:
        status = run_nicmgr(args, True)
    elif args.nicmgr_platform_model_server:
        status = run_nicmgr_platform_model_server(args, True)
    elif args.hal_test_fips:
        status = run_hal_test_fips(port, args)
        if status != 0:
            print "- hal_test failed, status=", status
    elif args.offload:
        status = run_offload_test(port, args)
        if status != 0:
            print "- offload_test failed, status=", status
    else:
        if args.mbt:
            mbt_port = find_port()
            print "* Using port (" + str(mbt_port) + ") for mbt\n"
            os.environ["MBT_GRPC_PORT"] = str(mbt_port)
            run_mbt(args, standalone=False)
        status = run_dol_test(args)

    if args.combined:
        if args.storage:
            status = run_storage_dol(port, args)
            if status != 0:
                print "- Storage dol failed, status=", status
        status = check_for_completion(nw_dol_process, stg_dol_process, model_process, hal_process, args)
        if status != 0:
            print "- Dol failed, status=", status

    if args.coveragerun:
        dump_coverage_data()
    os.chdir(nic_dir)
    if args.asmcov:
        call(["tools/run-coverage -k -c coverage_asm.json"], shell=True)

    # Get techsupport
    if args.feature in ['firewall']:
        os.chdir(halctl_dir)
        os.system(bin_dir+'/halctl show techsupport')
        os.chdir(nic_dir)

    try:
        os.remove(nic_dir + "/conf/pipeline.json")
    except:
        pass

    if args.nocleanup:
        print "Skipping cleanup"
    else:
        cleanup()

    call([print_cores_script])
    if not args.rtl and not args.no_error_check:
        ec = os.system("grep ERROR " + model_log)
        if not ec:
            print "ERRORs found in " + model_log
            status = 1
        else:
            print "PASS: No errors found in " + model_log
    print "Status = %d" % status
    os.system("echo %d > .run.status" % status)
    os.system("/sw/nic/tools/savelogs.sh")
    sys.exit(status)

def signal_handler(signal, frame):
    #print "cleanup from signal_handler"
    cleanup()
    sys.exit(1)

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler) # ctrl-c
    signal.signal(signal.SIGTERM, signal_handler) # kill
    signal.signal(signal.SIGQUIT, signal_handler) # ctrl-d
    signal.signal(signal.SIGABRT, signal_handler) # Assert or Abort
    signal.signal(signal.SIGSEGV, signal_handler) # Seg Fault
    remove_all_core_files()
    main()
