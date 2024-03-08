#pragma once
#include <unistd.h>
#include <rdma/rdma_cma.h>
#include <infiniband/verbs.h>
#include <string>
#include <signal.h>
#include <event2/event.h>
#include <misc.h>
#include <handler.h>


class Server;

enum Role : int32_t {
  Error,
  ServerConn,
  ClientConn,
};

enum State : int32_t {
  Vacant,              // Server & Client
  WaitingForRequest,   // Server
  WaitingForResponse,  // Client
  HandlingRequest,     // Server
};


class Connection {
public:

public:

  // for client, the cm_id is local,
  // for server, the cm_id is remote
  Connection(Role role, rdma_cm_id* cm_id, uint32_t n_buffer_page);
  ~Connection();

  rdma_conn_param copyConnParam();
  void setRkey(uint32_t rkey);
  rdma_cm_id* getCmId();
  uint32_t getLKey();
  uint32_t getRKey();
  void* getMRAddr();

  void postSend(void* local_addr, uint32_t length, uint32_t lkey, bool need_inline);
  void postRecv(void* local_addr, uint32_t length, uint32_t lkey);
  void lock();
  void unlock();
  
  void fillMR(void* data, uint32_t size);

  void prepare();
  void poll();
  void serverAdvance(const ibv_wc &wc);
  // void clientAdvance(const ibv_wc &wc);
  void* getMrBuffer() { return buffer_; }

private:
  static ibv_qp_init_attr defaultQpInitAttr();

  Role role_{Role::Error};
  State state_{State::Vacant};
  // for client, it presents the local cm_id,
  // for server, it presents the remote(client) cm_id
  rdma_cm_id* cm_id_;
  ibv_pd* local_pd_;
  ibv_cq* local_cq_;
  ibv_qp* local_qp_;
  uint32_t n_buffer_page_;
  void* buffer_;//mr buffer
  ibv_mr* buffer_mr_;
  rdma_conn_param param_;
  uint32_t rkey_;
  uint32_t lkey_;
  Spinlock lock_{};

  // hander;
  Handler handler_{};
};
