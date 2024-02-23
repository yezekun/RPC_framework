#pragma once
#include <stdint.h>//max clients nums
constexpr uint32_t MAX_NODE_NUM  = 8;//最大节点数
constexpr uint32_t MAX_CLIENTS_NUM  = 8;
constexpr uint32_t MAX_CONNECTION_NUM = 8;
constexpr uint32_t MAX_QUEUE_SIZE = 256;
constexpr uint32_t MAX_WORKER_NUM = 2;
constexpr uint32_t DEFAULT_CQ_CAPACITY = 64;
constexpr uint32_t MAX_SEND_WR_NUM = 64;
constexpr uint32_t MAX_RECV_WR_NUM = 64;
constexpr uint32_t BUFFER_PAGE_SIZE = 65536;
constexpr uint32_t DEFAULT_CONNECTION_TIMEOUT = 3000;
constexpr uint32_t MESSAGE_BUF_SIZE = 1024;