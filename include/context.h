#pragma once
#include <cstdint>

class Context {
public:
  Context(void *addr, uint32_t len) : addr_(addr), length_(len){};
  ~Context(){};

  void* addr(){
    return addr_;
  }
  uint32_t length(){
    return length_;
  }

private:
  void* addr_;
  uint32_t length_;
};