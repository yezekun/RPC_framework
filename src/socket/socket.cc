#include "socket/socket.hh"
#include "common/config.hh"
#include "socket/socket_message.hh"
#include <arpa/inet.h>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <sys/socket.h>
namespace RPC_FRAMEWORK::SOCKET {
auto socket_receive(int sock_fd, void *buffer, size_t len) -> ssize_t {
    ssize_t nr, tot_read;
    char *buf = (char *)buffer;
    tot_read = 0;

    while (len != 0 && (nr = read(sock_fd, buf, len)) != 0) {
        if (nr < 0) {
            RPC_FRAMEWORK::LOG::warn("Server socket receiver fail!");
            return -1;
        }
        len -= nr;
        buf += nr;
        tot_read += nr;
    }

    return tot_read;
};
auto socket_write(int sock_fd, void *buffer, size_t len) -> ssize_t {
    ssize_t nw, tot_written;
    const char *buf = (char *)buffer;

    for (tot_written = 0; tot_written < (ssize_t)len;) {
        nw = write(sock_fd, buf, len - tot_written);
        if (nw <= 0) {
            RPC_FRAMEWORK::LOG::warn("Client socket send fail!");
            return -1;
        }
        tot_written += nw;
        buf += nw;
    }
    return tot_written;
};
auto socket_server::socket_listen() -> int32_t {
    auto ret = bind(server_socket_fd_, (struct sockaddr *)&server_addr_,
                    sizeof(server_addr_));
    if (ret != 0) {
        RPC_FRAMEWORK::LOG::error("Server socket bind fail!");
        return -1;
    } else {
        RPC_FRAMEWORK::LOG::trace("Server socket bind success!");
    }

    ret = listen(server_socket_fd_,
                 RPC_FRAMEWORK::CONFIG::config::socket_server_listen_queue_num);
    if (ret != 0) {
        RPC_FRAMEWORK::LOG::error("Server socket listen fail!");
        return -1;
    } else {
        RPC_FRAMEWORK::LOG::trace("Server socket listen success!");
    }
    RPC_FRAMEWORK::LOG::info("Server socket listen begin!");
    // while (*close_bool) {
    while (1) {
        struct sockaddr_in client_addr {};
        socklen_t sockaddr_len = sizeof(struct sockaddr_in);
        auto client_socket_fd = accept(
            server_socket_fd_, (struct sockaddr *)&client_addr, &sockaddr_len);
        RPC_FRAMEWORK::LOG::trace("Server socket accept success!");
        socket_message message_buffer = {};
        ret = socket_receive(client_socket_fd, (char *)&message_buffer,
                             sizeof(socket_message));
        close(client_socket_fd);
        if (ret != sizeof(socket_message)) {
            RPC_FRAMEWORK::LOG::warn("Server socket recv message is not good");
        } else {
            socket_recv_message_queue::put_message(message_buffer);
            RPC_FRAMEWORK::LOG::info(
                "Server socket recv message:{} from {}", message_buffer.message,
                inet_ntoa(*((struct in_addr *)&message_buffer.src_node_ip)));
        }
    }
    close(server_socket_fd_);
    RPC_FRAMEWORK::LOG::info("Server socket closs success!");
    return 0;
};

auto socket_client::socket_client_send() -> int {
    // socket_send_message_queue message_queue;
    while (1) {
        client_socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket_fd_ < 0) {
            RPC_FRAMEWORK::LOG::error("Client socket create fail!");
        } else {
            RPC_FRAMEWORK::LOG::trace("Client socket create success!");
        }
        socket_message send_msg = socket_send_message_queue::get_message();

        struct sockaddr_in server_addr {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = send_msg.dst_node_ip;
        server_addr.sin_port = htons(send_msg.dst_node_port);
        auto ret = connect(client_socket_fd_, (struct sockaddr *)&server_addr,
                           sizeof(struct sockaddr_in));
        if (ret != 0) {
            RPC_FRAMEWORK::LOG::warn("Client socket connect fail!");
            send_msg.try_time--;
            if (send_msg.try_time)
                socket_send_message_queue::put_message(send_msg);
            continue;
        } else {
            RPC_FRAMEWORK::LOG::trace("Client socket connect success!");
        }

        ret = socket_write(client_socket_fd_, (char *)&send_msg,
                           sizeof(send_msg));
        if (ret != sizeof(send_msg)) {
            RPC_FRAMEWORK::LOG::warn("Client socket send message is not good");
            send_msg.try_time--;
            if (send_msg.try_time)
                socket_send_message_queue::put_message(send_msg);
            continue;
        }
        RPC_FRAMEWORK::LOG::info(
            "Client socket send message:{} to {}", send_msg.message,
            inet_ntoa(*((struct in_addr *)&send_msg.dst_node_ip)));
        close(client_socket_fd_);
    }
    return 0;
};
}; // namespace RPC_FRAMEWORK::SOCKET