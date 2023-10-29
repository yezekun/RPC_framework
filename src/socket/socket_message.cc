#include "socket/socket_message.hh"
#include <condition_variable>
#include <deque>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <thread>
namespace RPC_FRAMEWORK::SOCKET {

std::mutex socket_recv_message_queue::m_nutex;
std::condition_variable socket_recv_message_queue::m_cond;
std::queue<socket_message> socket_recv_message_queue::message_queue;

auto socket_recv_message_queue::put_message(socket_message msg) -> void {
    std::lock_guard<std::mutex> lock(m_nutex);
    message_queue.push(msg);
    m_cond.notify_all();
}

auto socket_recv_message_queue::get_message() -> socket_message {
    std::unique_lock<std::mutex> lock(m_nutex);
    while (message_queue.empty()) {
        m_cond.wait(lock);
    }
    socket_message msg = message_queue.front();
    message_queue.pop();
    lock.unlock();
    return msg;
}

std::mutex socket_send_message_queue::m_nutex;
std::condition_variable socket_send_message_queue::m_cond;
std::queue<socket_message> socket_send_message_queue::message_queue;

auto socket_send_message_queue::put_message(socket_message msg) -> void {
    std::lock_guard<std::mutex> lock(m_nutex);
    message_queue.push(msg);
    m_cond.notify_all();
}

auto socket_send_message_queue::get_message() -> socket_message {
    std::unique_lock<std::mutex> lock(m_nutex);
    while (message_queue.empty()) {
        m_cond.wait(lock);
    }
    socket_message msg = message_queue.front();
    message_queue.pop();
    lock.unlock();
    return msg;
}
}; // namespace RPC_FRAMEWORK::SOCKET