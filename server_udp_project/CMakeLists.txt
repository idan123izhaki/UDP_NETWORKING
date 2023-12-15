cmake_minimum_required(VERSION 3.26)
project(server_udp_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(server_udp_project main.cpp)

add_library(protobuf_lib ../fileStructure.pb.cc ../fileStructure.pb.h)

target_link_libraries(server_udp_project protobuf_lib protobuf)
