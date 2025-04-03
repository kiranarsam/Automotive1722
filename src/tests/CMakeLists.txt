project(Tests_Project)

# test receiver module
add_executable(test_receiver
  "src/test_receiver.cpp"
)

target_include_directories(test_receiver
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_receiver
  receiver
)

# test settings module
add_executable(test_settings
  "src/test_settings.cpp"
)

target_include_directories(test_settings
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_settings
  settings
)

# test transmitter module
add_executable(test_transmitter
  "src/test_transmitter.cpp"
)

target_include_directories(test_transmitter
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_transmitter
  transmitter
)

# test can-util dbc module
add_executable(test_dbc
  "src/test_dbc.cpp"
)

target_include_directories(test_dbc
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_dbc
  cancodec
  canutil
)

# test aggregator module
add_executable(test_aggregator
  "src/test_aggregator.cpp"
)

target_include_directories(test_aggregator
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_aggregator
  aggregator
)

# test data manager
add_executable(test_datamanager
  "src/test_datamanager.cpp"
)

target_include_directories(test_datamanager
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_datamanager
  datamanager
  agents
)

# test vss mapper module
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

# generate vss and dbc mapper json file
add_executable(generate_vss_json
  "src/generate_vss_json.cpp"
)

target_include_directories(generate_vss_json
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(generate_vss_json
  vssmapper
)

# test redis database connection
add_executable(test_redisdatabase
  "src/test_redisdatabase.cpp"
)

target_include_directories(test_redisdatabase
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_redisdatabase
  redisdatabase
)

# install the target binaries
install(TARGETS
    test_receiver
    test_settings
    test_transmitter
    test_dbc
    test_aggregator
    test_datamanager
    test_vssmapper
    generate_vss_json
    test_redisdatabase
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

# install config files
install(FILES
    ../../config/vss.yaml
    ../../config/vss_dbc_mapper.json
    ../../config/settings.dat
    ../../config/dbc.dbc
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
