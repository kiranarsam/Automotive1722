project(Mapper_Project)

add_library(vssmapper STATIC
  "src/VssMapper.cpp"
)

target_include_directories(vssmapper
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(vssmapper
  yaml-cpp
)
