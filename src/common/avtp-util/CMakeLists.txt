project(IeeeCan_Project)

add_library(avtp_util STATIC
  "src/AvtpUtil.cpp"
)

target_include_directories(avtp_util
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(avtp_util
  open1722
)