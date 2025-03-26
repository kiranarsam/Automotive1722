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
)

add_executable(generate_vss_json
  "src/main_gen_json.cpp"
)

target_include_directories(generate_vss_json
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(generate_vss_json
  vssmapper
)


install(TARGETS
    test_vssmapper
    generate_vss_json
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(FILES
    ./vss.yaml
    ./vss_dbc_mapper.json
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
