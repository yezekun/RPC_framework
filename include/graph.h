#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <const.h>
#include <log.h>
#include <limits>
// const int INF = std::numeric_limits<int>::max();  // 无穷大
struct nodeinformation{
  std::string host;
  std::string port;
  // Client *client_;
};
struct Edge {
    int destination;
    int weight;
};
class Graph {
private:
  int n, m;  // n 为节点数量，m 为边数量
   std::vector<std::vector<Edge>> g;
   std::vector<nodeinformation> nodeinform;
public:
  Graph(int node_num, int edge_num) {
    n = node_num;
    m = edge_num;
    g.resize(node_num);
    nodeinform.resize(node_num);
  }
  ~Graph(){}
  void add_graph(int graph_start,int graph_end,int weight){
    g[graph_start].push_back({graph_end, weight});
    info("add graph from %d to %d", graph_start,graph_end);
  }
  void add_node(int node_id,std::string host,std::string port){
    nodeinform[node_id].host = host;
    nodeinform[node_id].port = port;
    info("add node %d", node_id);
    // nodeinform[node_id].client_ = client;
  }

  void dijkstra(int start,std::vector<int> &dist, std::vector<int> &prev) {
    int n = g.size();
    dist.resize(n, std::numeric_limits<int>::max());
    prev.resize(n, -1); // 用于记录前驱节点
    dist[start] = 0;

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        std::greater<std::pair<int, int>>>
        pq;
    pq.push({0, start});

    while (!pq.empty()) {
      int u = pq.top().second;
      int d = pq.top().first;
      pq.pop();

      if (d > dist[u])
        continue;

      for (const Edge &edge : g[u]) {
        int v = edge.destination;
        int weight = edge.weight;

        if (dist[u] + weight < dist[v]) {
          dist[v] = dist[u] + weight;
          prev[v] = u; // 更新前驱节点
          pq.push({dist[v], v});
        }
      }
    }
  }

  // 用于从起点到目标节点重构最短路径
  std::vector<int> reconstructPath(const std::vector<int> &prev, int target) {
    std::vector<int> path;
    while (target != -1) {
      path.push_back(target);
      target = prev[target];
    }
    std::reverse(path.begin(), path.end());
    return path;
  }
};
