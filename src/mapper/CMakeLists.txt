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

add_executable(test_vssmapper
  "src/test_vssmapper.cpp"
)

target_include_directories(test_vssmapper
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_vssmapper
  vssmapper
  yaml-cpp
)

install(TARGETS
    test_vssmapper
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(FILES
    ./vss.yaml
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
