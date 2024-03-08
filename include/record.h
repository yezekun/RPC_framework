#pragma once
#ifndef RECORD_H
#define RECORD_H
#include <mutex>
#include <condition_variable>
#include <const.h>
class Client;
class Graph;
class Record {
public:
  static Client **clientArray_;
  static int node_id_;
  static Graph* graph_;

  // 
  // static std::mutex mutex[MAX_NODE_NUM];
  // static std::condition_variable cv[MAX_NODE_NUM];
  // static bool resp_ready[MAX_NODE_NUM];

};

#endif // RECORD_H