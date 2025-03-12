project(DataRouter_Project)

add_library(datarouter STATIC
  "src/Channel.cpp"
  "src/DataRouter.cpp"
)

target_include_directories(datarouter
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(datarouter
  receiver
  transmitter
)