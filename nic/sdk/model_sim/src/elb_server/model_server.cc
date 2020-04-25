#include <python2.7/Python.h>
#include "model_sim/include/scapy_pkt_gen.h"
#include "model_sim/include/elb_env_base.h"
#include "model_sim/include/cpu.h"
#include <iomanip>
#include <zmq.h>
#include "model_sim/include/HBM.h"
#include "model_sim/include/HOST_MEM.h"
#include "model_sim/include/buf_hdr.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <assert.h>
#include <vector>
#include <queue>
#include <signal.h>
//#include "nic/utils/host_mem/params.hpp"
#include "model_sim/include/host_mem_params.hpp"

#ifdef COVERAGE
#define HAL_GCOV_FLUSH()     { ::__gcov_flush(); }
#else
#define HAL_GCOV_FLUSH()     { }
#endif

elb_env_base *g_env;
std::queue<std::vector<uint8_t>> g_cpu_pkts;
extern "C" void __gcov_flush();
namespace utils {

class HostMem : public pen_mem_base {
 public:
  HostMem() {
    shmid_ = shmget(HostMemHandle(), kShmSize, IPC_CREAT | 0666);
    if (shmid_ < 0) {
        perror("shmget");
        printf("shmget failed for %lu size\n", kShmSize);
    }
    printf("shmid_ val %d\n", shmid_);
    assert(shmid_ >= 0);
    shmaddr_ = shmat(shmid_, nullptr, 0);
    assert(shmaddr_ != (void*)-1);
  }
  virtual ~HostMem() {
    if ((shmaddr_) && (shmaddr_ != (void *)-1))
      shmdt(shmaddr_);
    shmctl(shmid_, IPC_RMID, NULL);
  }
  virtual bool burst_read(uint64_t addr, unsigned char *data,
                          unsigned int len, bool secure, bool reverse_bo) {
    uint64_t orig_addr = addr;
    addr &= (1ULL << 52) - 1;
    for (unsigned int i = 0; i < match_addr.size(); i++) {
       if ((addr == (match_addr[i] & ((1ULL << 52) - 1))) && (orig_addr != match_addr[i])) {
         return false;
       }
    }
    if ((addr >= kShmSize) || ((addr + len) > kShmSize))
      return false;
    void *sa = (void *)((uint64_t)shmaddr_ + addr);
    bcopy(sa, data, len);
    return true;
  }
  virtual bool burst_write(uint64_t addr, const unsigned char *data,
                           unsigned int len, bool secure, bool reverse_bo) {
    uint64_t orig_addr = addr;
    addr &= (1ULL << 52) - 1;
    for (unsigned int i = 0; i < match_addr.size(); i++) {
       if ((addr == (match_addr[i] & ((1ULL << 52) - 1))) && (orig_addr != match_addr[i])) {
         return false;
       }
    }
    if ((addr >= kShmSize) || ((addr + len) > kShmSize))
      return false;
    void *da = (void *)((uint64_t)shmaddr_ + addr);
    bcopy(data, da, len);
    return true;
  }
  void set_match_addr(uint64_t addr) {
    match_addr.push_back(addr);
  }

 private:
  int shmid_;
  void *shmaddr_;
  vector<uint64_t> match_addr;
};

}  // namespace utils

utils::HostMem g_host_mem;

static void dumpHBM (void) {
      auto it = HBM::access()->begin();
      auto lst = HBM::access()->end();
      while (it != lst) {
          uint64_t addr = (*it);
          std::cout << std::hex << "Addr 0x" << addr <<
                    std::hex << " data 0x" << HBM::access()->get<uint32_t>(addr)
                    << std::dec << std::endl;
          it++;
      }
      return;
}

void process_buff (buffer_hdr_t *buff, elb_env_base *env) {
    switch (buff->type) {
        case BUFF_TYPE_STEP_TIMER_WHEEL:
        {
            /* Call step timer wheel update in model */
            env->step_tmr_wheel_update(buff->slowfast, buff->ctime);
            buff->type = BUFF_TYPE_STATUS;
            buff->status = 0;
    	    std::cout << "step_tmr_wheel_update slowfast: " << buff->slowfast << " ctime: " << buff->ctime << std::endl;
        }
            break;
        case BUFF_TYPE_STEP_PKT:
        {
            std::vector<unsigned char> pkt_vector(buff->data, buff->data + buff->size);
            uint32_t port = 0;
            /* Send packet through the model */
            env->step_network_pkt(pkt_vector, buff->port);
            buff->type = BUFF_TYPE_STATUS;
            buff->status = 0;
    	    std::cout << "step_network_pkt port: " << port << " size: " << buff->size << std::endl;
        }
            break;
        case BUFF_TYPE_GET_NEXT_PKT:
        {
            std::vector<uint8_t> out_pkt;
            uint32_t port;
            uint32_t cos;
            /* Get output packet from the model */
            if (!env->get_next_pkt(out_pkt, port, cos)) {
    	        //std::cout << "get_next_pkt: no packet" << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                buff->size = out_pkt.size();
                buff->port = port;
                buff->cos = cos;
                memcpy(buff->data, out_pkt.data(), out_pkt.size());
    	        std::cout << "get_next_pkt port: " << port << " cos: " << cos << " size: " << buff->size << std::endl;
            }
        }
            break;
        case BUFF_TYPE_REG_READ:
        {
            uint32_t data;
            uint64_t addr;
            addr = buff->addr;
            bool ret = env->read_reg(addr, data);
            ret = true;
            if (!ret) {
    	        std::cout << "ERROR: Reading register" << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                buff->size = sizeof(uint32_t);
                memcpy(buff->data, &data, buff->size);
                printf("read_reg addr: 0x%lx data: 0x%x\n", addr, data);
            }
        }
            break;
        case BUFF_TYPE_REG_WRITE:
        {
            uint32_t data;
            uint64_t addr = buff->addr;
            memcpy(&data, buff->data, sizeof(uint32_t));
            bool ret = env->write_reg(addr, data);
            ret = true;
            if (!ret) {
    	        std::cout << "ERROR: Writing register" << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                buff->type = BUFF_TYPE_STATUS;
                buff->status = 0;
                printf("write_reg addr: 0x%lx  data: 0x%x\n", addr, data);
            }
        }
            break;
        case BUFF_TYPE_MEM_READ:
        {
            uint64_t addr = buff->addr;
            bool ret = env->read_mem(addr, buff->data, buff->size);
            ret = true;
            if ((buff->size > MODEL_ZMQ_BUFF_SIZE) || !ret) {
    	        std::cout << "ERROR: Reading memory" << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                printf("read_mem addr: 0x%lx size: %d\n", addr, buff->size);
            }
        }
            break;
        case BUFF_TYPE_MEM_WRITE:
        {
            uint64_t addr = buff->addr;
            bool ret = env->write_mem(addr, buff->data, buff->size);
            ret = true;
            if ((buff->size > MODEL_ZMQ_BUFF_SIZE) || !ret) {
    	        std::cout << "ERROR: Writing memory" << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                buff->type = BUFF_TYPE_STATUS;
                buff->status = 0;
                printf("write_mem addr: 0x%lx size: %d\n", addr, buff->size);
            }
        }
            break;
        case BUFF_TYPE_MEM_WRITE_PCIE:
        {
            uint64_t addr = buff->addr;
            bool ret = env->write_mem_pcie(addr, buff->data, buff->size);
            ret = true;
            if ((buff->size > MODEL_ZMQ_BUFF_SIZE) || !ret) {
                std::cout << "ERROR: Writing pcie memory" << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                buff->type = BUFF_TYPE_STATUS;
                buff->status = 0;
                printf("write_mem_pcie addr: 0x%lx size: %d\n", addr, buff->size);
            }
        }
            break;
        case BUFF_TYPE_DOORBELL:
        {
            uint64_t data;
            uint64_t addr = buff->addr;
            memcpy(&data, buff->data, sizeof(uint64_t));
            env->step_doorbell(addr, data);
            buff->type = BUFF_TYPE_STATUS;
            buff->status = 0;
    	    std::cout << "step_doorbell addr: " << std::hex << addr <<
                         " data: " << std::hex << data << std::endl;
        }
            break;
        case BUFF_TYPE_HBM_DUMP:
        {
            std::cout << "*************** HBM dump START ***************" << std::endl;
            dumpHBM();
            std::cout << "*************** HBM dump END ***************" << std::endl;
        }
            break;
        case BUFF_TYPE_STEP_CPU_PKT:
        {
            std::vector<uint8_t> pkt_vector(buff->data, buff->data + buff->size);

            g_cpu_pkts.push(pkt_vector);
            buff->type = BUFF_TYPE_STATUS;
            buff->status = 0;
    	    std::cout << "step_cpu_pkt size: " << buff->size << std::endl;
        }
            break;
        case BUFF_TYPE_GET_NEXT_CPU_PKT:
        {
            std::vector<uint8_t> out_pkt;
            if (g_cpu_pkts.empty()) {
    	        std::cout << "get_next_cpu_packet: no packet in queue." << std::endl;
                buff->type = BUFF_TYPE_STATUS;
                buff->status = -1;
            } else {
                out_pkt = g_cpu_pkts.front();
                g_cpu_pkts.pop();
                buff->size = out_pkt.size();
                memcpy(buff->data, out_pkt.data(), out_pkt.size());
    	        std::cout << "get_next_cpu_pkt size: " << buff->size << std::endl;
            }
        }
            break;

        case BUFF_TYPE_MAC_CFG:
            {
                bool ret = true;
                if (!ret) {
                    std::cout << "ERROR: MAC CFG" << std::endl;
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = -1;
                } else {
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = 0;
                }
            }
            break;

        case BUFF_TYPE_MAC_EN:
            {
                bool ret = true;
                if (!ret) {
                    std::cout << "ERROR: MAC EN" << std::endl;
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = -1;
                } else {
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = 0;
                }
            }
            break;

        case BUFF_TYPE_MAC_SOFT_RESET:
            {
                bool ret = true;
                if (!ret) {
                    std::cout << "ERROR: MAC SOFT RESET" << std::endl;
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = -1;
                } else {
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = 0;
                }
            }
            break;

        case BUFF_TYPE_MAC_STATS_RESET:
            {
                bool ret = true;
                if (!ret) {
                    std::cout << "ERROR: MAC STATS RESET" << std::endl;
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = -1;
                } else {
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = 0;
                }
            }
            break;

        case BUFF_TYPE_MAC_INTR_EN:
            {
                bool ret = true;
                if (!ret) {
                    std::cout << "ERROR: MAC INTR EN/DISABLE" << std::endl;
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = -1;
                } else {
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = 0;
                }
            }
            break;

        case BUFF_TYPE_MAC_INTR_CLR:
            {
                bool ret = true;
                if (!ret) {
                    std::cout << "ERROR: MAC INTR CLEAR" << std::endl;
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = -1;
                } else {
                    buff->type = BUFF_TYPE_STATUS;
                    buff->status = 0;
                }
            }
            break;

        case BUFF_TYPE_REGISTER_MEM_ADDR:
            {
                g_host_mem.set_match_addr(buff->addr);
                std::cout << std::hex << "Registered address: 0x" << buff->addr << std::endl;
            }
            break;

         case BUFF_TYPE_EXIT_SIM:
            {
	        buff->type = BUFF_TYPE_STATUS;
	        buff->status = 0;
            }
            break;

         case BUFF_TYPE_CONFIG_DONE:
            {
	        buff->type = BUFF_TYPE_STATUS;
	        buff->status = 0;
            }
            break;

         case BUFF_TYPE_TESTCASE_BEGIN:
            {
                // Overloading size parameter to pass the tcid
                printf("============== Starting Testcase TC%06d-%04d ===============\n",
                       buff->tcid, buff->loopid);
    	        buff->type = BUFF_TYPE_STATUS;
	            buff->status = 0;
            }
            break;

         case BUFF_TYPE_TESTCASE_END:
            {
                // Overloading size parameter to pass the tcid
                printf("============== Ending Testcase TC%06d-%04d ===============\n",
                       buff->size, buff->loopid);
    	        buff->type = BUFF_TYPE_STATUS;
	            buff->status = 0;
            }
            break;
         case BUFF_TYPE_EOS_IGNORE_ADDR:
            {
                // Overloading size parameter to pass the tcid
                printf("eos_ignore_addr: %#016lx %d\n", buff->addr, buff->size);
    	        buff->type = BUFF_TYPE_STATUS;
	            buff->status = 0;
            }
            break;


        case BUFF_TYPE_STATUS:
        default:
            assert(0);
            break;
    }
    return;
}

static void wait_loop() {
    int             rc;
    buffer_hdr_t    *buff;
    char            *zmqsockstr = NULL;
    char            recv_buff[MODEL_ZMQ_BUFF_SIZE];

    zmqsockstr = model_utils_get_zmqsockstr();

    //  ZMQ Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    rc = zmq_bind(responder, zmqsockstr);
    assert (rc == 0);
    std::cout << "Model initialized! Waiting for pkts/command...." << std::endl;
    while (1) {
        rc = zmq_recv (responder, recv_buff, MODEL_ZMQ_BUFF_SIZE, 0);
        assert(rc != -1);
        buff = (buffer_hdr_t *) recv_buff;
        process_buff(buff, g_env);
        rc = zmq_send (responder, recv_buff, MODEL_ZMQ_BUFF_SIZE, 0);
        assert(rc != -1);
    }
    return;
}

static void
model_sig_handler (int sig)
{
    printf("Rcvd signal %u\n", sig);

    switch (sig) {
    case SIGUSR1:
    case SIGUSR2:
        std::cout << "Rcvd SIGUSR1/SIGUSR2, flushing code coverage data ..." << std::endl;
        fflush(stdout);
        HAL_GCOV_FLUSH();
        wait_loop();
        break;

    default:
        exit(0);
        break;
    }
    return;
}

static void
model_sig_init (void)
{
    struct sigaction action;
    action.sa_handler = model_sig_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGUSR1, &action, NULL);
    sigaction (SIGUSR2, &action, NULL);
}

int main (int argc, char ** argv)
{
    model_sig_init();

    HOST_MEM::access(&g_host_mem);

    sknobs_init(argc, argv);
    auto env = new elb_env_base(0);
    env->init();
    std::cout << "init successful" << std::endl;
    env->load_debug();
    std::cout << "load_debug successful" << std::endl;
    //env->enable_padding();
    std::cout << "enable_padding successful" << std::endl;
    g_env = env;
    wait_loop();

    return 0;
}
