cmake_minimum_required(VERSION 3.20)

project(Comm_Project)

add_library(comm_conn STATIC
    "src/ether_common.c"
    "src/can_common.c"
)

target_include_directories(comm_conn
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)