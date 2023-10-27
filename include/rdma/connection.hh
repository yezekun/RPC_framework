#pragma once
#include <bitset>
#include <rdma/rdma_cma.h>
#include <stdio.h>
namespace RPC_FRAMEWORK::RDMA {
class connection {
  private:
    rdma_cm_id *cm_id_;
    uint16_t src_node_id_;
    uint16_t dst_node_id_;

    ibv_qp *qp_;
    uint32_t qp_num_;

    ibv_mr *mr_;

  public:
    connection(rdma_cm_id *cm_id, uint16_t src_node_id, uint16_t dst_node_id,
               ibv_qp *qp, uint32_t qp_num, ibv_mr *mr) {
        cm_id_ = cm_id;
        src_node_id_ = src_node_id;
        dst_node_id_ = dst_node_id;
        qp_ = qp;
        qp_num_ = qp_num;
        mr_ = mr;
    };
    ~connection(){

    };
    [[nodiscard]] auto cm_id() -> rdma_cm_id * { return cm_id_; };
    [[nodiscard]] auto src_node_id() -> uint16_t { return src_node_id_; };
    [[nodiscard]] auto dst_node_id() -> uint16_t { return dst_node_id_; };
    [[nodiscard]] auto qp() -> ibv_qp * { return qp_; };
    [[nodiscard]] auto qp_num() -> uint16_t { return qp_num_; };
    [[nodiscard]] auto mr() -> ibv_mr * { return mr_; };
};

}; // namespace RPC_FRAMEWORK::RDMA