#pragma once
#include<vector>
#include<record.h>
#include<iostream>
#include<stdio.h>
#include<string>
#include <client.h>
#include<server.h>
#include<graph.h>

class Node
{
private:
  /* data */  
  int node_id_;
  int node_num_;
  bool * node_connection_;//判断是否有连接
  std::vector<std::string> node_host_vector_;
  std::vector<std::string> node_port_vector_;
  
  Server *server_;
  Graph *graph_;
public:
  Node(int node_id, int node_num,
        bool *node_connection,
        std::vector<std::string> node_host_vector,
        std::vector<std::string> node_port_vector) {
    node_id_ = node_id;
    Record::node_id_ = node_id;
    node_num_ = node_num;
    node_connection_ = node_connection;
    node_host_vector_ = node_host_vector;
    node_port_vector_ = node_port_vector;
  }
  ~Node() {
    // TODO ~NODE()
    //  delete client_;
    //  delete server_;
    //  delete clientArray_;
  }
  void downloadGraph(Graph *graph) { graph_ = graph; }
  void nodeServerRun(){
    server_ = new Server("0.0.0.0", node_port_vector_[node_id_].c_str());
    server_->run();
    info("server run successfully");
  }
  void nodeClientRun(){
    for (int i = 0; i < node_num_; i++) {
        info("node_connection_[%d] = %s", i, node_connection_[i] ? "true" : "false");
    }
    for (int i = 0; i < node_num_;i++){
      if(node_connection_[i]){
        Record::clientArray_[i] = new Client;
        info("client %d ,addr %p", i, Record::clientArray_[i]);
        Record::clientArray_[i]->connect(node_host_vector_[i].c_str(),
                                         node_port_vector_[i].c_str());
        info("connect node %d successfully",i);
      }else{
        Record::clientArray_[i] = nullptr;
      }
    }
  }
  void sendRequest(char* msg, uint32_t src_node_id,uint32_t dst_node_id,uint32_t msglen,void* resp_addr){
    /* 寻找最短路 */
    if(node_connection_[dst_node_id]){// case 1: 可直达
      info("该信息可直达 from %d to %d",node_id_,dst_node_id);
      Record::clientArray_[dst_node_id]->sendRequest(msg, src_node_id,dst_node_id,dst_node_id,msglen,resp_addr);
    }
    else{
      info("该信息需转发 from %d to %d",node_id_,dst_node_id);
      int start_node = node_id_;
      std::vector<int> dist;  
      std::vector<int> prev;
      graph_->dijkstra(start_node, dist, prev);//TODO 对各个终点进行分析，可优化
      // for (int i = 0; i < node_num_; ++i) {
      std::cout << "最短路径 from " << start_node << " to " << dst_node_id << ": ";
      if (dist[dst_node_id] == std::numeric_limits<int>::max()) {// case 2: 不可到达
        std::cout << "不可到达" << std::endl;
      } else {// case 3: 转发后可到达
        std::vector<int> path = graph_->reconstructPath(prev, dst_node_id);
        for (int j = 0; j < path.size(); ++j) {
          std::cout << path[j];
          if (j != path.size() - 1)
            std::cout << " -> ";
        }
        std::cout << std::endl;
        Record::clientArray_[path[1]]->sendRequest(msg,src_node_id,path[1],dst_node_id,msglen,resp_addr);
      }
      // }
    }
  }
  int get_node_id() { return node_id_; }

};


// int start = 0;
// std::vector<int> dist;
// std::vector<int> prev;
// dijkstra(graph, start, dist, prev);
// // 输出最短路径
// for (int i = 0; i < node_num; ++i) {
//   std::cout << "Shortest path from " << start << " to " << i << ": ";
//   if (dist[i] == INF) {
//     std::cout << "Not reachable" << std::endl;
//   } else {
//     std::vector<int> path = reconstructPath(prev, i);
//     for (int j = 0; j < path.size(); ++j) {
//       std::cout << path[j];
//       if (j != path.size() - 1)
//         std::cout << " -> ";
//     }
//     std::cout << std::endl;
//   }
// }