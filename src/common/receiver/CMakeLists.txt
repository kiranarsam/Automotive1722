project(Receiver_Project)

add_library(receiver STATIC
  "src/Receiver.cpp"
)

target_include_directories(receiver
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(receiver
  comm_if
  avtp_util
  common_utils
  canwriter
  datahandler
)
