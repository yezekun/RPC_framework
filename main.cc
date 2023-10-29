#include "common/log.hh"
#include "node/node.hh"
#include "socket/socket.hh"
#include "socket/socket_message.hh"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
struct test_socket_thread_data {
    int node_id;
    int thread_id;
    int port;
};
void *test_socket_server(void *args) {
    RPC_FRAMEWORK::SOCKET::socket_server receiver(
        ((test_socket_thread_data *)args)->node_id,
        ((test_socket_thread_data *)args)->port);
    receiver.socket_listen();
    pthread_exit(NULL);
}
void *test_socket_client(void *args) {
    RPC_FRAMEWORK::SOCKET::socket_client socket_client(
        ((test_socket_thread_data *)args)->node_id,
        ((test_socket_thread_data *)args)->port);
    socket_client.socket_client_send();
    pthread_exit(NULL);
}
auto main(int argc, char **argv) -> int {
    RPC_FRAMEWORK::LOG::GlobalLogger::init(spdlog::level::trace);
    RPC_FRAMEWORK::LOG::info("socket node id: 0 is server; 1 is client");
    int socket_node_id;
    scanf("%d", &socket_node_id);

    /* test socket part begin*/
    RPC_FRAMEWORK::LOG::info("socket part test begin");
    pthread_t threads[2];
    if (socket_node_id == 0) { // socket server 0
        test_socket_thread_data data0{
            node_id : 0,
            thread_id : 0,
            port : 5000,
        };
        auto ret = pthread_create(&threads[0], NULL, test_socket_server,
                                  (void *)&data0);
        if (ret) {
            RPC_FRAMEWORK::LOG::trace("Unable to create thread 0");
        } else {
            RPC_FRAMEWORK::LOG::trace("Create thread 0 success");
        }
        // /* second pthread*/
        // test_socket_thread_data data1{
        //     node_id : 0,
        //     thread_id : 1,
        //     flag : &flag,
        //     port : 5001,
        // };
        // ret = pthread_create(&threads[1], NULL, test_socket_server,
        //                      (void *)&data1);
        // if (ret) {
        //     RPC_FRAMEWORK::LOG::trace("Unable to create thread 0");
        // } else {
        //     RPC_FRAMEWORK::LOG::trace("Create thread 0 success");
        // }
        sleep(5); // sleep 10s

        RPC_FRAMEWORK::LOG::trace("Thread quit success");
    } else if (socket_node_id == 1) { // socket client 1
        /* Create some information and throw it into the queue */
        RPC_FRAMEWORK::SOCKET::socket_message msg;
        msg.dst_node_ip = inet_addr("127.0.0.1");
        msg.src_node_ip = inet_addr("127.0.0.1");
        msg.dst_node_port = 5000;
        msg.try_time = 3;
        strcpy(msg.message, "msg 1");
        RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        strcpy(msg.message, "msg 2");
        RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        strcpy(msg.message, "msg 3");
        RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        strcpy(msg.message, "msg 4");
        RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        strcpy(msg.message, "msg 5");
        RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        // msg.dst_node_port = 5001;
        // strcpy(msg.message, "msg 1");
        // RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        // strcpy(msg.message, "msg 2");
        // RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        // strcpy(msg.message, "msg 3");
        // RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        // strcpy(msg.message, "msg 4");
        // RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        // strcpy(msg.message, "msg 5");
        // RPC_FRAMEWORK::SOCKET::socket_send_message_queue::put_message(msg);
        test_socket_thread_data data0{
            node_id : 1,
            thread_id : 0,
            port : 6000,
        };
        auto ret = pthread_create(&threads[0], NULL, test_socket_client,
                                  (void *)&data0);
        if (ret) {
            RPC_FRAMEWORK::LOG::trace("Unable to create thread 0");
        } else {
            RPC_FRAMEWORK::LOG::trace("Create thread 0 success");
        }
        // test_socket_thread_data data1{
        //     node_id : 1,
        //     thread_id : 1,
        //     flag : &flag,
        //     port : 6001,
        // };
        // ret = pthread_create(&threads[1], NULL, test_socket_client,
        //                      (void *)&data1);
        // if (ret) {
        //     RPC_FRAMEWORK::LOG::trace("Unable to create thread 0");
        // } else {
        //     RPC_FRAMEWORK::LOG::trace("Create thread 0 success");
        // }
        sleep(5); // sleep 10s
        RPC_FRAMEWORK::LOG::trace("Thread quit success");
    }
    /* test socket part over*/
    RPC_FRAMEWORK::LOG::trace("test overover!");
    _exit(0);
}
