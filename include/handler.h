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

  Message handlerRequest(Message* req){//解析
    // info("resp_addr: %p", req->getRespAddr());
    info("\n发送过程耗时:%d ns", req->GetTimeDifference());
    if (req->msgType() == ImmRequest || req->msgType() == Request) {
      int src_node_id = req->getSrcNode();
      int dst_node_id = req->getDstNode();
      info("收到信息:\n from node %d to node %d \n msg is %s", src_node_id,
           dst_node_id, req->dataAddr());
      if (dst_node_id == Record::node_id_) { // 目标节点就是该节点
        char data[req->dataLen()];
        memcpy(data, req->dataAddr(), req->dataLen());
        std::sort(data, data + sizeof(data));
        info("处理完毕，返回: %s", data);
        Message resp(data,src_node_id,dst_node_id, sizeof(data), Response,req->getRespAddr());
        resp.set_temp_node(req->get_temp_node());
        return resp;
        } else {//目标节点不是该节点
          info("转发req");
          //TODO  case 1: 转发但可直达
          int start_node = Record::node_id_;
          std::vector<int> dist;  
          std::vector<int> prev;
          Record::graph_->dijkstra(start_node, dist, prev);
          // for (int i = 0; i < node_num_; ++i) {
          std::cout << "最短路径 from " << start_node << " to " << dst_node_id << ": ";
          if (dist[dst_node_id] == std::numeric_limits<int>::max()) {// case 2: 不可到达
            std::cout << "不可到达" << std::endl;
            exit(-1);
          } else {// case 3: 转发后可到达
            std::vector<int> path = Record::graph_->reconstructPath(prev, dst_node_id);
            for (int j = 0; j < path.size(); ++j) {
              std::cout << path[j];
              if (j != path.size() - 1)
                std::cout << " -> ";
            }
            std::cout << std::endl;
            char* resp_addr = new char[req->dataLen()];
            std::unique_lock<std::mutex> lock(Record::mutex[path[1]]);
            // std::unique_lock<std::mutex> lock;
            Record::resp_ready[path[1]] = false;
            Record::clientArray_[path[1]]->sendRequest(
                req->dataAddr(), src_node_id, path[1], dst_node_id,
                req->dataLen(), resp_addr);
            info("上锁%d", path[1]);
            // 等待响应，resp_addr会变化
            Record::cv[path[1]].wait(lock, [path]() { return Record::resp_ready[path[1]]; });
             
            // char temp[10] = "123456789";
            info("转发resp，返回: %s", resp_addr);
            Message resp(resp_addr, src_node_id, dst_node_id,req->dataLen(), Response,req->getRespAddr());
            resp.set_temp_node(req->get_temp_node());
            return resp;
          }
        }
    } else {
      info("receive error message!");
      exit(-1);
    }
  }

private:
};