project(Agents_Project)

add_library(agents STATIC
  "src/CanAgent.cpp"
  "src/AgentFactory.cpp"
)

target_include_directories(agents
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(agents
  datahandler
  redisdatabase
  vssmapper
)