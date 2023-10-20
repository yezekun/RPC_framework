cmake_minimum_required(VERSION 3.10)
project(rpc_framework)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_definitions("-Wall -g")

set(COMMON_DIR ./include/common)
set(NODE_DIR ./include/node)
set(RDMA_DIR ./include/rdma)
set(RPC_DIR ./include/rpc)

# 编译子文件夹的CMakeLists.txt
# add_subdirectory(node)
# add_subdirectory(rdma)
# add_subdirectory(rpc)

# 规定.h头文件路径
include_directories("./include")

# 生成可执行文件
add_executable(${PROJECT_NAME} "./main.cc")

# 链接操作
# target_link_libraries(${PROJECT_NAME}
#     general_pb2)
