# project(app)
# file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
# # add_executable(server server.cc)
# # add_executable(client client.cc)

# # target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
# # target_link_libraries(server PUBLIC rdma-lib)

# # target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
# # target_link_libraries(client PUBLIC rdma-lib)

# add_executable(node node.cc c)
# target_include_directories(node PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
# target_link_libraries(node PUBLIC rdma-lib -lyaml-cpp)

project(app)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
add_executable(node ${SOURCE_FILES} node.cc)
target_include_directories(node PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_link_libraries(node PUBLIC rdma-lib -lyaml-cpp)