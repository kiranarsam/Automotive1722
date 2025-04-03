project(Transmitter_Project)

add_library(transmitter STATIC
  "src/Transmitter.cpp"
)

target_include_directories(transmitter
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(transmitter
  comm_if
  avtp_util
  common_utils
  canreader
)
