project(IeeeCan_Project)

add_library(ieee1722 STATIC
  "src/Ieee1722Receiver.cpp"
  "src/Ieee1722Transmitter.cpp"
  "src/IeeeUtil.cpp"
)

target_include_directories(ieee1722
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(ieee1722
  comm_conn
  open1722
)

add_executable(test_receiver
  "src/main_rx.cpp"
)

target_include_directories(test_receiver
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_receiver
  ieee1722
)

add_executable(test_transmitter
  "src/main_tx.cpp"
)

target_include_directories(test_transmitter
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_transmitter
  ieee1722
)
