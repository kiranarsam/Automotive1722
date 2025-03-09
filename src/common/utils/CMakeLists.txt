
project(Utils_Project)

add_library(common_utils STATIC
  "src/CommonUtils.cpp"
)

target_include_directories(common_utils
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
