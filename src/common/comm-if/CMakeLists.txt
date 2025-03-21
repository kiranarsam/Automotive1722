cmake_minimum_required(VERSION 3.20)

project(CommIf_Project)

add_library(comm_if STATIC
    "src/ether_comm_if.c"
    "src/can_comm_if.c"
    "src/lo_comm_if.c"
)

target_include_directories(comm_if
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)