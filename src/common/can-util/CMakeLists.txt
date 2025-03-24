
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

add_executable(test_dbc
  "main_dbc.cpp"
)

target_include_directories(test_dbc
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(test_dbc
  cancodec
  canutil
)

install(TARGETS
    test_dbc
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(FILES
    ./dbc.dbc
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
