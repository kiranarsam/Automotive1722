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
