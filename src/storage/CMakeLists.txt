project(Storage_Project)

add_library(redisdatabase STATIC
  "src/RedisConnection.cpp"
  "src/RedisDatabase.cpp"
)

target_include_directories(redisdatabase
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(redisdatabase
  hiredis
)


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

install(TARGETS
    test_redisdatabase
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
