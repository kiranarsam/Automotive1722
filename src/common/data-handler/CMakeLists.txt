project(DataHandler_Project)

add_library(datahandler STATIC
  "src/DataCallbackHandler.cpp"
)

target_include_directories(datahandler
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)