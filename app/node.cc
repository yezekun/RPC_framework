#include <const.h>
#include <graph.h>
#include <iostream>
#include <node.h>
#include <string>
#include <log.h>
#include <fstream>
#include <client.h>
#include <yaml-cpp/yaml.h>
#include <random>
#include <message.h>
Node *node;

std::string generateRandomString(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis('a', 'z');

    std::string randomString;
    randomString.reserve(n);

    for (int i = 0; i < n; ++i) {
        randomString.push_back(static_cast<char>(dis(gen)));
    }

    return randomString;
}
void* run_server(void* args)
{
  node->nodeServerRun();
  return 0;
}

void* run_client(void* args)
{
  std::cout << "运行 client" << std::endl;
  node->nodeClientRun();
  sleep(10);
  char tempstr[10];
  Message temp(tempstr, 1, 1, 1, MessageType::ImmRequest, args);
  if(node->get_node_id()==0){
    char resp_addr[10][1025];
    std::string msgtemp = generateRandomString(1024);
    for (int i = 0; i < 2;i++){

      // info("\n\n进入发送 %d 循环:%d ns",i, temp.GetTimeDifference());
      // TODO 修改
      info("send %d message", i);
      uint32_t src_node_id = node->get_node_id();
      uint32_t dst_node_id = node->get_node_id()+1;
      // std::string msgtemp = generateRandomString(1024);
      msgtemp[1024] = 0, resp_addr[i][1024] = 0;
      info("发送 %d 的开始:%d ns",i, temp.GetTimeDifference());
      node->sendRequest((char *)msgtemp.c_str(), src_node_id, dst_node_id,
                        msgtemp.length(), (void *)resp_addr[i]);
      info("发送 %d 结束:%d ns\n\n",i, temp.GetTimeDifference());
    }
    sleep(30);
    // for (int i = 0; i < 2;i++){
    //   info("%d 收到回复: %s\n\n\n",i, resp_addr[i]);
    // }

  }
  sleep(30);
  return 0;
}
 
int main([[gnu::unused]] int argc, char *argv[]) {
  /* 开始读取配置,并初始化拓扑图*/
  int node_id = atoi(argv[1]);
  info("本节点编号为: %d",node_id);
  std::ifstream file("../../config/node_config.yaml");
  YAML::Node config = YAML::Load(file);
  int node_num = config["node_num"].as<int>();
  int edge_num = config["edge_num"].as<int>();
  Graph graph(node_num, edge_num);
  Record::graph_ = &graph;
  std::cout << node_num << std::endl;
  // std::string node_host[node_num];
  std::vector<std::string> node_host_vector =
    config["node_host"].as<std::vector<std::string>>();
  std::vector<std::string> node_port_vector =
    config["node_port"].as<std::vector<std::string>>();
  // std::cout << node_host_vector[0] << std::endl;
  // std::cout << node_port_vector[0] << std::endl;
  // Server *server_pointer[node_num];
  // Client *client_pointer[node_num];
  // for (int i = 0; i < node_num; i++) {
  //   server_pointer[i] = nullptr;
  //   client_pointer[i] = nullptr;
  // }
  YAML::Node nodeEdgeList = config["node_edge"];
  bool* node_connection = new bool[node_num];
  for (int i = 0; i < node_num; i++) {
      node_connection[i] = false;//初始化为未连接
      graph.add_node(i, node_host_vector[i], node_port_vector[i]);
  }
  for(const auto& item : nodeEdgeList){
    int edge_src, edge_dst;
    for (const auto& pair : item) {
      edge_src = pair.first.as<int>();
      edge_dst = pair.second.as<int>();
      if (edge_src == node_id) {
        node_connection[edge_dst] = true;
      }
      graph.add_graph(edge_src, edge_dst, 1);
    }
  }
  for (int i = 0; i < node_num; i++) {
    info("node_connection[%d] = %s", i, node_connection[i] ? "true" : "false");
  }
  node = new Node(node_id, node_num,node_connection, node_host_vector, node_port_vector);
  node->downloadGraph(&graph);
  pthread_t server_tids;
  int ret = pthread_create(&server_tids, NULL, run_server, NULL);
  sleep(12);
  pthread_t client_tids;
  ret = pthread_create(&client_tids, NULL, run_client, NULL);
  sleep(100);
  // delete node;
}

// record static
Graph* Record::graph_ = nullptr;
Client ** Record::clientArray_ = new Client*[MAX_NODE_NUM];
int Record::node_id_ = -1;
// bool Record::resp_ready[MAX_NODE_NUM] = {false};
// std::mutex Record::mutex[MAX_NODE_NUM]{};
// std::condition_variable Record::cv[MAX_NODE_NUM]{};
