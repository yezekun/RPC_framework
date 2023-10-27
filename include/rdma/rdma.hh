#pragma once
#include <bitset>
#include <rdma/rdma_cma.h>
#include <stdio.h>
namespace RPC_FRAMEWORK::RDMA {
class rdma_server {
  private:
  public:
    auto connect(const char *dst_host, const char *dst_port,
                 uint16_t dst_node_id) -> void{};
};
}; // namespace RPC_FRAMEWORK::RDMA