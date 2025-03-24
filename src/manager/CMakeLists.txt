project(Manager_Project)

add_library(datamanager STATIC
  "src/DataManager.cpp"
)

target_include_directories(datamanager
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(datamanager
  aggregator
  datahandler
  canutil
  cancodec
)

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

install(TARGETS
    test_datamanager
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
