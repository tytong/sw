#include <stdint.h>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include <memory>
#include <ctime>
#include "gen/proto/internal.pb.h"
#include "gen/proto/internal.grpc.pb.h"
#include "gen/proto/interface.pb.h"
#include "gen/proto/interface.grpc.pb.h"
#include "gen/proto/internal.pb.h"
#include "gen/proto/internal.grpc.pb.h"
#include "dol/iris/test/storage/hal_if.hpp"
#include "gflags/gflags.h"
#include "nic/sdk/storage/storage_seq_common.h"

#define HAL_WAIT_READY_TIMEOUT_SECS 1000

DECLARE_uint64(hal_port);
DECLARE_string(hal_ip);

const static uint32_t  kDefaultQStateSize   = 64;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using internal::Internal;
using intf::Interface;
using internal::Internal;

std::shared_ptr<Channel> hal_channel;

namespace {

std::unique_ptr<Internal::Stub> internal_stub;
std::unique_ptr<Interface::Stub> interface_stub;
std::unique_ptr<Internal::Stub> crypto_stub;
std::unique_ptr<Internal::Stub> brings_stub;

}  // anonymous namespace

namespace hal_if {

void init_hal_if() {
  if (internal_stub.get() != nullptr)
    return;

  char host_addr[32];
  std::time_t start, end, elapsed;
  grpc_connectivity_state state = GRPC_CHANNEL_INIT;

  sprintf(host_addr, "%s:%lu", FLAGS_hal_ip.c_str(), FLAGS_hal_port);
  hal_channel = std::move(grpc::CreateChannel(
      host_addr, grpc::InsecureChannelCredentials()));

  OFFL_FUNC_INFO("Waiting for HAL channel ready");
  for (start = std::time(nullptr), end = start, elapsed = 0; 
       elapsed < HAL_WAIT_READY_TIMEOUT_SECS;
       end = std::time(nullptr)) {

      state = hal_channel->GetState(true);
      elapsed = end - start;
      if (state == GRPC_CHANNEL_READY) {
          break;
      }

      // Wait for State change or deadline
      usleep(10000);
      hal_channel->WaitForStateChange(state,
                               gpr_time_from_seconds(1, GPR_TIMESPAN));
  }
  if (state == GRPC_CHANNEL_READY) {
      OFFL_FUNC_INFO("HAL channel became ready in {} seconds",
                     (int)elapsed);
  } else {
      OFFL_FUNC_INFO("HAL channel never reached ready state after {} seconds",
                     (int)elapsed);
  }

  std::unique_ptr<Internal::Stub> int_stub(
      Internal::NewStub(hal_channel));
  internal_stub = std::move(int_stub);

  std::unique_ptr<Interface::Stub> if_stub(
      Interface::NewStub(hal_channel));
  interface_stub = std::move(if_stub);

  std::unique_ptr<Internal::Stub> crypt_stub(
    Internal::NewStub(hal_channel));
  crypto_stub = std::move(crypt_stub);

  std::unique_ptr<Internal::Stub> bring_stub(
    Internal::NewStub(hal_channel));
  brings_stub = std::move(bring_stub);
}

// Start with a base lif id and count up
#define STORAGE_DOL_SW_LIF_BASE 1900
static uint64_t curr_lif_id = STORAGE_DOL_SW_LIF_BASE;

int create_lif(lif_params_t *params, uint64_t *ret_lif_id) {
  grpc::ClientContext context;
  intf::LifRequestMsg req_msg;
  intf::LifResponseMsg resp_msg;

  if (!params || !ret_lif_id)
    return -1;

  auto req = req_msg.add_request();
  if (!params->sw_lif_id) {
    params->sw_lif_id = curr_lif_id++;
  }
  req->mutable_key_or_handle()->set_lif_id(params->sw_lif_id);

  for (int i = 0; i < LIF_MAX_TYPES; i++) {
    if (params->type[i].valid) {
      if (params->type[i].queue_size < LIF_BASE_QUEUE_SIZE)
        return -1;
      auto map = req->add_lif_qstate_map();
      map->set_type_num(i);
      map->set_size(params->type[i].queue_size - LIF_BASE_QUEUE_SIZE);
      map->set_entries(params->type[i].num_queues);
      map->set_purpose(
          static_cast<::intf::LifQPurpose>(params->type[i].queue_purpose));
    }
  }
  if (params->rdma_enable) {
    req->set_enable_rdma(true);
    req->set_rdma_max_keys(params->rdma_max_keys);
    req->set_rdma_max_ahs(params->rdma_max_ahs);
    req->set_rdma_max_pt_entries(params->rdma_max_pt_entries);
  }

  auto status = interface_stub->LifCreate(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ? 
  // TODO: Check status
  *ret_lif_id = resp_msg.response(0).status().hw_lif_id();
  return 0;
}

int set_lif_bdf(uint32_t hw_lif_id, uint32_t bdf_id) {
  grpc::ClientContext context;
  internal::ConfigureLifBdfRequestMsg req_msg;
  internal::ConfigureLifBdfResponseMsg resp_msg;

  auto req = req_msg.add_request();
  req->set_lif(hw_lif_id);
  req->set_bdf(bdf_id);

  auto status = internal_stub->ConfigureLifBdf(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ? 
  if (resp_msg.response(0).status() != 0) return -1;
  else return 0;
}

int
get_lif_info(uint32_t sw_lif_id, uint64_t *ret_hw_lif_id)
{
    intf::LifGetResponse rsp;
    intf::LifGetRequest *req __attribute__((unused));
    intf::LifGetRequestMsg req_msg;
    intf::LifGetResponseMsg rsp_msg;
    grpc::ClientContext context;

    req = req_msg.add_request();
    req->mutable_key_or_handle()->set_lif_id(sw_lif_id);
    auto status = interface_stub->LifGet(&context, req_msg, &rsp_msg);
    if (status.ok()) {
        for (int i = 0; i < rsp_msg.response().size(); i++) {
            rsp = rsp_msg.response(i);
            if (rsp.api_status() == types::API_STATUS_OK) {
                OFFL_FUNC_INFO("Lif {} hw_lif_id {:#x}",
                               sw_lif_id, rsp.status().hw_lif_id());
                *ret_hw_lif_id = rsp.status().hw_lif_id();
                return 0;
            }
        }
    }

    return -1;
}

int get_pgm_base_addr(const char *prog_name, uint64_t *base_addr) {
  grpc::ClientContext context;
  internal::GetProgramAddressRequestMsg req_msg;
  internal::ProgramAddressResponseMsg resp_msg;

  if (!prog_name || !base_addr)
    return -1;

  auto req = req_msg.add_request();
  req->set_handle("p4plus");
  req->set_prog_name(prog_name);
  req->set_resolve_label(false);

  auto status = internal_stub->GetProgramAddress(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ? 
  *base_addr = resp_msg.response(0).addr();
  return 0;
}

int get_pgm_label_offset(const char *prog_name, const char *label, uint8_t *off) {
  grpc::ClientContext context;
  internal::GetProgramAddressRequestMsg req_msg;
  internal::ProgramAddressResponseMsg resp_msg;

  if (!prog_name || !label || !off)
    return -1;

  auto req = req_msg.add_request();
  req->set_handle("p4plus");
  req->set_prog_name(prog_name);
  req->set_resolve_label(true);
  req->set_label(label);

  auto status = internal_stub->GetProgramAddress(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ? 
  *off = ((resp_msg.response(0).addr()) >> 6) & 0xFF;
  return 0;
}

int get_lif_qstate_addr(uint32_t lif, uint32_t qtype, uint32_t qid, uint64_t *qaddr) {
  grpc::ClientContext context;
  intf::GetQStateRequestMsg req_msg;
  intf::GetQStateResponseMsg resp_msg;

  if (!qaddr) 
    return -1;

  auto req = req_msg.add_reqs();
  req->set_lif_handle(lif);
  req->set_type_num(qtype);
  req->set_qid(qid);

  auto status = interface_stub->LifGetQState(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ?
  if (resp_msg.resps(0).error_code()) {
    OFFL_FUNC_ERR("error: {}", resp_msg.resps(0).error_code());
    return -1;
  }

  *qaddr = resp_msg.resps(0).q_addr();
  return 0;
}

int get_lif_qstate(uint32_t lif, uint32_t qtype, uint32_t qid, uint8_t *qstate) {
  grpc::ClientContext context;
  intf::GetQStateRequestMsg req_msg;
  intf::GetQStateResponseMsg resp_msg;

  if (!qstate) 
    return -1;

  auto req = req_msg.add_reqs();
  OFFL_FUNC_INFO("lif {} type {} qid {}", lif, qtype, qid);
  req->set_lif_handle(lif);
  req->set_type_num(qtype);
  req->set_qid(qid);
  req->set_ret_data_size(kDefaultQStateSize);

  auto status = interface_stub->LifGetQState(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ?
  if (resp_msg.resps(0).error_code())
    return -1;

  memcpy(qstate, resp_msg.resps(0).queue_state().c_str(), kDefaultQStateSize);
  return 0;
}

int set_lif_qstate_size(uint32_t lif, uint32_t qtype, uint32_t qid, uint8_t *qstate, uint32_t size) {
  grpc::ClientContext context;
  intf::SetQStateRequestMsg req_msg;
  intf::SetQStateResponseMsg resp_msg;

  if (!qstate) {
    OFFL_FUNC_INFO("q state null");
    return -1;
  }

  auto req = req_msg.add_reqs();
  req->set_lif_handle(lif);
  req->set_type_num(qtype);
  req->set_qid(qid);
  req->set_queue_state((const char *) qstate, size);

  auto status = interface_stub->LifSetQState(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ?
  if (resp_msg.resps(0).error_code()) {
    OFFL_FUNC_ERR("error {}", resp_msg.resps(0).error_code());
    return -1;
  }

  return 0;
}

int set_lif_qstate(uint32_t lif, uint32_t qtype, uint32_t qid, uint8_t *qstate) {
  return set_lif_qstate_size(lif, qtype, qid, qstate, kDefaultQStateSize);
}

int alloc_hbm_address(const char *handle, uint64_t *addr, uint32_t *size) {
  grpc::ClientContext context;
  internal::AllocHbmAddressRequestMsg req_msg;
  internal::AllocHbmAddressResponseMsg resp_msg;

  if (!addr || !size) 
    return -1;

  auto req = req_msg.add_request();
  req->set_handle(handle);

  auto status = internal_stub->AllocHbmAddress(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ? 
  *addr = resp_msg.response(0).addr();
  *size = resp_msg.response(0).size();
  return 0;
}

int get_xts_ring_base_address(bool is_decr, uint64_t *addr, bool is_gcm) {
  grpc::ClientContext context;

  internal::AllocHbmAddressRequestMsg req_msg;
  internal::AllocHbmAddressResponseMsg resp_msg;

  if (!addr)
    return -1;

  auto req = req_msg.add_request();
  if(is_decr)
    if(!is_gcm) req->set_handle("brq-ring-xts1");
    else req->set_handle("brq-ring-gcm1");
  else
    if(!is_gcm) req->set_handle("brq-ring-xts0");
    else req->set_handle("brq-ring-gcm0");

  auto status = internal_stub->AllocHbmAddress(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed: {}", status.error_message());
    return -1;
  }

  // TODO: Check number of responses ?
  *addr = resp_msg.response(0).addr();

  return 0;
}

int get_ring_meta_config(types::BarcoRings ring_type,
                         barco_ring_meta_config_t *meta)
{
  grpc::ClientContext context;
  internal::BarcoGetRingMetaConfigRequestMsg req_msg;
  internal::BarcoGetRingMetaConfigResponseMsg resp_msg;
  auto req = req_msg.add_request();
  req->set_ring_type(ring_type);

  auto status = crypto_stub->BarcoGetRingMetaConfig(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("BarcoGetRingMetaConfig request failed");
    return -1;
  }

  meta->ring_base = resp_msg.response(0).ring_base();
  meta->opaque_tag_addr = resp_msg.response(0).opaque_tag_addr();
  meta->shadow_pndx_addr = resp_msg.response(0).shadow_pndx_addr();
  meta->producer_idx_addr = resp_msg.response(0).producer_idx_addr();
  meta->pndx_size = resp_msg.response(0).pndx_size();
  meta->desc_size = resp_msg.response(0).desc_size();
  meta->ring_size = resp_msg.response(0).ring_size();
  meta->opaque_tag_size = resp_msg.response(0).opaque_tag_size();
  return 0;
}

int get_key_index(char* key, types::CryptoKeyType key_type, uint32_t key_size, uint32_t* key_index) {
  grpc::ClientContext context, context2;
  *key_index = 0xffffffff;
  internal::CryptoKeyCreateRequestMsg cr_req_msg;
  internal::CryptoKeyCreateResponseMsg cr_resp_msg;
  cr_req_msg.add_request();

  auto status = crypto_stub->CryptoKeyCreate(&context, cr_req_msg, &cr_resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("Create request failed");
    return -1;
  }
  if(cr_resp_msg.response(0).keyindex() == *key_index) {
    OFFL_FUNC_ERR("Create request failed");
    return -1;
  }
  *key_index = cr_resp_msg.response(0).keyindex();

  internal::CryptoKeyUpdateRequestMsg upd_req_msg;
  internal::CryptoKeyUpdateResponseMsg upd_resp_msg;
  auto upd_req = upd_req_msg.add_request();
  internal::CryptoKeySpec* spec =   upd_req->mutable_key();
  spec->set_key(key, key_size);
  spec->set_keyindex(*key_index);
  spec->set_key_size(key_size);
  spec->set_key_type(key_type);

  status = crypto_stub->CryptoKeyUpdate(&context2, upd_req_msg, &upd_resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("Update request failed");
    return -1;
  }
  if(upd_resp_msg.response(0).keyindex() != *key_index) {
    OFFL_FUNC_ERR("Expected keyindex {} recvd {}", *key_index, upd_resp_msg.response(0).keyindex());
    OFFL_FUNC_ERR("Update request failed");
    return -1;
  }

  return 0;
}

int delete_key(uint32_t key_index) {
  grpc::ClientContext context;
  internal::CryptoKeyDeleteRequestMsg del_req_msg;
  internal::CryptoKeyDeleteResponseMsg del_resp_msg;
  auto req = del_req_msg.add_request();
  req->set_keyindex(key_index);
  auto status = crypto_stub->CryptoKeyDelete(&context, del_req_msg, &del_resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("Delete request failed");
    return -1;
  }
  if(del_resp_msg.response(0).keyindex() != key_index) {
    OFFL_FUNC_ERR("Delete request failed with bad keyindex");
    return -1;
  }
  return 0;
}

int get_asym_key_index(const uint8_t *key_desc,
                       uint32_t key_size,
                       uint32_t *key_index) {
  grpc::ClientContext context, context2;
  *key_index = 0xffffffff;
  internal::CryptoAsymKeyCreateRequestMsg cr_req_msg;
  internal::CryptoAsymKeyCreateResponseMsg cr_resp_msg;
  cr_req_msg.add_request();

  auto status = crypto_stub->CryptoAsymKeyCreate(&context, cr_req_msg, &cr_resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("Create request failed");
    return -1;
  }
  *key_index = cr_resp_msg.response(0).keyindex();

  internal::CryptoAsymKeyWriteRequestMsg wr_req_msg;
  internal::CryptoAsymKeyWriteResponseMsg wr_resp_msg;
  auto wr_req = wr_req_msg.add_request();
  internal::CryptoAsymKeySpec* spec = wr_req->mutable_key();
  spec->set_key(key_desc, key_size);
  spec->set_keyindex(*key_index);

  status = crypto_stub->CryptoAsymKeyWrite(&context2, wr_req_msg, &wr_resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("Write request failed");
    return -1;
  }
  if(wr_resp_msg.response(0).keyindex() != *key_index) {
    OFFL_FUNC_ERR(" Expected keyindex {} recvd {}", *key_index, wr_resp_msg.response(0).keyindex());
    OFFL_FUNC_ERR("Write request failed");
    return -1;
  }
  return 0;
}

int delete_asym_key(uint32_t key_index) {
  grpc::ClientContext context;
  internal::CryptoAsymKeyDeleteRequestMsg del_req_msg;
  internal::CryptoAsymKeyDeleteResponseMsg del_resp_msg;
  auto req = del_req_msg.add_request();
  req->set_keyindex(key_index);
  auto status = crypto_stub->CryptoAsymKeyDelete(&context, del_req_msg, &del_resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("Delete request failed");
    return -1;
  }
  if(del_resp_msg.response(0).keyindex() != key_index) {
    OFFL_FUNC_ERR("Delete request failed with bad keyindex");
    return -1;
  }
  return 0;
}

using internal::GetOpaqueTagAddrRequestMsg;
using internal::GetOpaqueTagAddrResponseMsg;
int get_xts_opaque_tag_addr(bool is_decr, uint64_t* addr, bool is_gcm) {
  grpc::ClientContext context;
  GetOpaqueTagAddrRequestMsg req_msg;
  GetOpaqueTagAddrResponseMsg resp_msg;
  auto req = req_msg.add_request();
  if(is_decr)
    if(!is_gcm) req->set_ring_type(types::BARCO_RING_XTS1);
    else req->set_ring_type(types::BARCO_RING_GCM1);
  else
    if(!is_gcm) req->set_ring_type(types::BARCO_RING_XTS0);
    else req->set_ring_type(types::BARCO_RING_GCM0);
  auto status = brings_stub->GetOpaqueTagAddr(&context, req_msg, &resp_msg);
  if (!status.ok()) {
    OFFL_FUNC_ERR("failed");
    return -1;
  }
  *addr = resp_msg.response(0).opaque_tag_addr();

  return 0;
}

}  // namespace hal_if
