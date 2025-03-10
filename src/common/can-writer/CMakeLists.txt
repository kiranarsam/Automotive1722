
project(CanWriter_Project)

add_library(canwriter STATIC
  "src/CanWriter.cpp"
)

target_include_directories(canwriter
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(canwriter
  comm_if
)