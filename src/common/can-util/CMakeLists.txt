
project(CanUtil_Project)

add_library(canutil STATIC
  "src/CanDbc.cpp"
  "src/ProcessCanMessage.cpp"
)

target_include_directories(canutil
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(canutil
  cancodec
)
