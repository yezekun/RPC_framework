#include "socket/socket.hh"
#include "common/config.hh"
#include <arpa/inet.h>
#include <sys/socket.h>
namespace RPC_FRAMEWORK::SOCKET {

auto socket_server::socket_listen(bool *close_bool) -> int32_t {
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
    while (*close_bool) {
        struct sockaddr_in client_addr {};
        socklen_t sockaddr_len = sizeof(struct sockaddr_in);
        auto client_socket_fd = accept(
            server_socket_fd_, (struct sockaddr *)&client_addr, &sockaddr_len);
        RPC_FRAMEWORK::LOG::trace("Server socket accept success!");
        // TODO ipc_pool
        auto message_buffer =
            malloc(RPC_FRAMEWORK::CONFIG::config::socket_message_max_size);
        auto ret = socket_receive(
            server_socket_fd_, message_buffer,
            RPC_FRAMEWORK::CONFIG::config::socket_message_max_size);
        if (ret == -1) {
            //  TODO 忽略message
        }

        RPC_FRAMEWORK::LOG::info("Server socket recv{}",
                                 (char *)message_buffer);
        // TODO 处理message
        free(message_buffer);
    }
    close(server_socket_fd_);
    RPC_FRAMEWORK::LOG::info("Server socket closs success!");
    return 0;
};
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

auto socket_client::socket_client_connect() -> int {

    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd < 0) {
        RPC_FRAMEWORK::LOG::error("Client socket create fail!");
        return -1;
    } else {
        RPC_FRAMEWORK::LOG::trace("Client socket create success!");
    }
    // TODO conection
};
auto socket_client::socket_send(int sock_fd, void *buffer, size_t len)
    -> ssize_t {
    ssize_t nw, tot_written;
    const char *buf = (char *)buffer;

    for (tot_written = 0; tot_written < len;) {
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
}; // namespace RPC_FRAMEWORK::SOCKET