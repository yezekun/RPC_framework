#pragma once
#include <stdint.h>
#include <string>
#include <const.h>
#include <chrono>
// struct send_message
// {
//   int src_node_id;
//   int dst_node_id;
//   char msg[MESSAGE_BUF_SIZE - sizeof(src_node_id) - sizeof(dst_node_id)];
//   /* data */
// };
class [[gnu::packed]] BufferMeta {
public:
   char buf_[MESSAGE_BUF_SIZE]{};
};

enum MessageType {
  Dummy,
  Request,
  ImmRequest,
  Response,
};

class [[gnu::packed]] Header {
public:
  uint32_t src_node_id_;
  uint32_t dst_node_id_;
  uint32_t data_len_{};
  MessageType type_{Dummy};
  void *resp_addr_;
  // int temp_node;
  // std::chrono::time_point<std::chrono::system_clock,std::chrono::nanoseconds> start_time_;
  int64_t start_time_integer_;  // 使用整数类型存储时间点信息
};

class [[gnu::packed]] Message {
public:
  explicit Message(char* buf,  uint32_t src_node_id,uint32_t dst_node_id, uint32_t len, MessageType type,void *resp_addr) {
    printf("create a new message!create a new message!create a new message!\n");
    SetStartTime(std::chrono::high_resolution_clock::now());
    memcpy(meta_.buf_, buf, len);
    header_.src_node_id_ = src_node_id;
    header_.dst_node_id_= dst_node_id;
    header_.data_len_ = len;
    header_.type_ = type;
    header_.resp_addr_ = resp_addr;
  };
  ~Message(){};
  char* dataAddr()      { return meta_.buf_;}
  void *getRespAddr()   { return header_.resp_addr_; }

  void setSrcNode(uint32_t node_id ) {header_.src_node_id_=node_id; }
  void setDstNode(uint32_t node_id ) {header_.dst_node_id_=node_id; }
  uint32_t getSrcNode() { return header_.src_node_id_; }
  uint32_t getDstNode() { return header_.dst_node_id_; }

  uint32_t dataLen()    { return header_.data_len_; }
  
  void setMsgType(MessageType type) { header_.type_ = type; }
  MessageType msgType() { return header_.type_; }
  // void set_temp_node(int temp) { header_.temp_node = temp; }
  // int get_temp_node() { return header_.temp_node; }
  void SetStartTime(const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& time) {
    header_.start_time_integer_ = time.time_since_epoch().count();
  }
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> GetStartTime() const {
    return std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(
        std::chrono::nanoseconds(header_.start_time_integer_));
  } 
  std::chrono::nanoseconds GetTimeDifference() const {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> start_time = GetStartTime();
    std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
    std::chrono::nanoseconds diff = current_time - start_time;
    return diff;
  }
private:
  Header header_{};
  BufferMeta meta_;
};

