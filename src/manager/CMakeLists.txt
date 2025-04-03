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
