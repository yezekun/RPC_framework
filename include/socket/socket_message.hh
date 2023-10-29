#pragma once
#include "common/config.hh"
#include "common/log.hh"
#include <arpa/inet.h>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <thread>
namespace RPC_FRAMEWORK::SOCKET {
struct socket_message {
    // message_type
    uint8_t src_node_id;
    in_addr_t src_node_ip;

    uint8_t dst_node_id;
    in_addr_t dst_node_ip;
    uint16_t dst_node_port;

    char message[RPC_FRAMEWORK::CONFIG::config::socket_message_max_size];
    size_t message_size;
    uint8_t try_time;
};
class socket_recv_message_queue {

  private:
    static std::mutex m_nutex;
    static std::condition_variable m_cond;
    static std::queue<socket_message> message_queue;

  public:
    static auto put_message(socket_message msg) -> void;

    static auto get_message() -> socket_message;
};
class socket_send_message_queue {

  private:
    static std::mutex m_nutex;
    static std::condition_variable m_cond;
    static std::queue<socket_message> message_queue;

  public:
    static auto put_message(socket_message msg) -> void;

    static auto get_message() -> socket_message;
};
}; // namespace RPC_FRAMEWORK::SOCKET