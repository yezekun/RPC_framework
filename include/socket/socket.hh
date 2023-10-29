#pragma once
#include "common/log.hh"
#include "socket/socket_message.hh"
#include <arpa/inet.h>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
namespace RPC_FRAMEWORK::SOCKET {
class socket_server {
  private:
    uint8_t socket_server_id_;
    sockaddr_in server_addr_;
    int32_t server_socket_fd_;

  public:
    socket_server(uint8_t socket_server_id, uint16_t port) {
        socket_server_id_ = socket_server_id;

        server_addr_.sin_family = AF_INET; // IPv4 网络协议的套接字类型
        server_addr_.sin_addr.s_addr =
            htons(INADDR_ANY); // 32位IP地址 此处接收任何传入消息的地址
        server_addr_.sin_port = htons(port);

        server_socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_fd_ < 0) {
            RPC_FRAMEWORK::LOG::error("Server socket create fail!");
        } else {
            RPC_FRAMEWORK::LOG::trace("Server socket create success!");
        }
    };
    auto socket_listen() -> int32_t;
};

class socket_client {
  private:
    uint8_t socket_client_id_;
    uint32_t port_;
    int32_t client_socket_fd_;

  public:
    socket_client(uint8_t socket_client_id, uint32_t port) {
        socket_client_id_ = socket_client_id;
        port_ = port;
    };
    auto socket_client_send() -> int; // 0 success;1 fail
};
}; // namespace RPC_FRAMEWORK::SOCKET