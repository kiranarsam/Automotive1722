
project(CanReader_Project)

add_library(canreader STATIC
  "src/CanReader.cpp"
)

target_include_directories(canreader
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(canreader
  comm_if
)