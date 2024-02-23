#include <client.h>
#include <log.h>
#include <const.h>
#include <iostream>
#include <mutex>
#include <message.h>
#include <connection.h>
Client::Client() {
  cm_event_channel_ = rdma_create_event_channel();//创建接收事件的通道
  checkNotEqual(cm_event_channel_, static_cast<rdma_event_channel*>(nullptr), "rdma_create_event_channel() failed");
}

Client::~Client() {
  // rdma_disconnect will generate an event and a IBV_WC_SEND
  poller_.stop();
  int ret = rdma_disconnect(cm_id_);
  wCheckEqual(ret, 0, "rdma_disconnect() failed to disconnect");
  rdma_cm_event* cm_event = waitEvent(RDMA_CM_EVENT_DISCONNECTED);
  wCheckNotEqual(cm_event, static_cast<rdma_cm_event*>(nullptr), "failed to get disconnection event");
  ret = rdma_ack_cm_event(cm_event);
  wCheckEqual(ret, 0, "rdma_ack_cm_event() failed to send ack");
  freeaddrinfo(dst_addr_);
  rdma_destroy_event_channel(cm_event_channel_);
  poller_.deregisterConn();
  info("client disconnect");
}

void Client::connect(const char* host, const char* port) {

  int ret = 0;
  ret = rdma_create_id(cm_event_channel_, &cm_id_, nullptr, RDMA_PS_TCP);//分配一个rdma_cm_id，类似于socket。
  checkEqual(ret, 0, "rdma_create_id() failed");

  ret = getaddrinfo(host, port, nullptr, &dst_addr_);
  checkEqual(ret, 0, "getaddrinfo() failed");

  // When the resolution is completed, the cm_event_channel_ will generate an cm_event. 
  ret = rdma_resolve_addr(cm_id_, nullptr, dst_addr_->ai_addr, DEFAULT_CONNECTION_TIMEOUT);  //获取本地RDMA设备以到达远程地址。
  checkEqual(ret, 0, "rdma_resolve_addr() failed");
  rdma_cm_event* cm_event = waitEvent(RDMA_CM_EVENT_ADDR_RESOLVED);//等待RDMA_CM_EVENT_ROUTE_RESOLVED事件。
  checkNotEqual(cm_event, static_cast<rdma_cm_event*>(nullptr), "failed to resolve the address");
  ret = rdma_ack_cm_event(cm_event);//确认事件
  checkEqual(ret, 0, "rdma_ack_cm_event() failed to send ack");
  info("address resolution is completed");

  // same as above, resolve the route
  ret = rdma_resolve_route(cm_id_, DEFAULT_CONNECTION_TIMEOUT);//确定到达远程地址的路由
  checkEqual(ret, 0, "rdma_resolve_route failed");
  cm_event = waitEvent(RDMA_CM_EVENT_ROUTE_RESOLVED);//等待RDMA_CM_EVENT_ROUTE_RESOLVED事件。
  checkNotEqual(cm_event, static_cast<rdma_cm_event*>(nullptr), "failed to resolve the route");
  ret = rdma_ack_cm_event(cm_event);//确认事件
  checkEqual(ret, 0, "rdma_ack_cm_event() failed to send ack");
  info("route resolution is completed");
  
  setupConnection(cm_id_, 1);

  // run poller
  poller_.run();
}

rdma_cm_event* Client::waitEvent(rdma_cm_event_type expected) {
  rdma_cm_event* cm_event = nullptr;
  int ret = rdma_get_cm_event(cm_event_channel_, &cm_event);//等待RDMA_CM_EVENT_ROUTE_RESOLVED事件。
  if (ret != 0) {
    info("fail to get cm event");
    return nullptr;
  }
  if (cm_event->status != 0) {
    info("get a bad cm event");
    return nullptr;
  }
  else if (cm_event->event != expected) {
    info("got: %s, expected: %s", rdma_event_str(cm_event->event),
         rdma_event_str(expected));
    return nullptr;
  }
  else
    return cm_event;
}

void Client::setupConnection(rdma_cm_id* client_id, uint32_t n_buffer_page) {
  Connection* conn = new Connection(Role::ClientConn, client_id, n_buffer_page);
  rdma_conn_param param = conn->copyConnParam();
  int ret = rdma_connect(client_id, &param);
  checkEqual(ret, 0, "rdma_connect() failed");
  rdma_cm_event* ev = waitEvent(RDMA_CM_EVENT_ESTABLISHED);
  checkNotEqual(ev, static_cast<rdma_cm_event*>(nullptr), "fail to establish the connection");
  info("connection is established");

  poller_.registerConn(conn);
}

void Client::sendRequest(char* msg,uint32_t src_node_id,uint32_t next_node_id,uint32_t dst_node_id, uint32_t msglen,void* resp_addr) {
  Message req(msg, src_node_id, dst_node_id, msglen, MessageType::ImmRequest,resp_addr);
  req.set_temp_node(next_node_id);

  poller_.sendRequest(req);
}


// /* ClientPoller */
ClientPoller::ClientPoller() {

}

ClientPoller::~ClientPoller() {

}

void ClientPoller::registerConn(Connection* conn) {
  std::lock_guard<Spinlock> lock(lock_);
  conn_ = conn;
}

void ClientPoller::deregisterConn() {
  std::lock_guard<Spinlock> lock(lock_);
  delete conn_;
}

void ClientPoller::sendRequest(Message req) {
  conn_->lock(); // unlock when receive response;
  conn_->fillMR((void*)&req, sizeof(req));
  // info("post send reqeust, req data is: %s", req.dataAddr());

  // info("复制时间：%d", req.GetTimeDifference());
  // req.SetStartTime(std::chrono::high_resolution_clock::now());
  // info("修改时间调用耗时：%d", req.GetTimeDifference());
  // req.SetStartTime(std::chrono::high_resolution_clock::now());

  conn_->postSend(conn_->getMRAddr(), sizeof(req),  conn_->getLKey(), false);
}


void ClientPoller::run() {
  running_.store(true, std::memory_order_release);
  poll_thread_ = std::thread(&ClientPoller::poll, this);
  info("start running client poller");
}

void ClientPoller::stop() {
  if (running_.load(std::memory_order_acquire)) {
    running_.store(false, std::memory_order_release);
    poll_thread_.join();
    info("connection poller stopped");
  }
}

void ClientPoller::poll() {
  while (running_.load(std::memory_order_acquire)) {
    std::lock_guard<Spinlock> lock(lock_);
    conn_->poll();
  }
}