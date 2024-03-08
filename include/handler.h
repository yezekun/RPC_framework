#pragma once
#include <message.h>
#include <graph.h>
#include <record.h>
#include <log.h>
#include <client.h>
#include <algorithm>

class Handler {
public:
  Handler(){};
  ~Handler(){};

  Message* handlerRequest(Message* msg){//解析
    info("resp_addr: %p", msg->getRespAddr());
    // if (req->msgType() == ImmRequest || req->msgType() == Request) {
    int src_node_id = msg->getSrcNode();
    int dst_node_id = msg->getDstNode();
    info("收到request信息:\n from node %d to node %d \n msg is %s", src_node_id,
          dst_node_id, msg->dataAddr());
    if (dst_node_id == Record::node_id_) { // 目标节点就是该节点
      ////char data[req->dataLen()];
      ////memcpy(data, req->dataAddr(), req->dataLen());
      std::sort(msg->dataAddr(), msg->dataAddr() + msg->dataLen());
      info("处理完毕，准备返回: %s", msg->dataAddr());
      msg->setSrcNode(dst_node_id);
      msg->setDstNode(src_node_id);
      msg->setMsgType(MessageType::Response);
      // Message *resp = new Message(data, src_node_id, dst_node_id, sizeof(data),
      //                             Response, req->getRespAddr());
      // resp.set_temp_node(req->get_temp_node());
      return msg;
    } else {//目标节点不是该节点
      info("转发msg");
      int next_node_id =
          Record::graph_->next_node_id(Record::node_id_, dst_node_id);
      if(next_node_id == -1){
        exit(-1);
      }else{
        Record::clientArray_[next_node_id]->sendRequest(msg);
            // msg->dataAddr(), src_node_id, next_node_id, dst_node_id,
            // msg->dataLen(), msg->getRespAddr());
      }
      return nullptr;
    }
    
  }

private:
};