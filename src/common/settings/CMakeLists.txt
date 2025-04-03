cmake_minimum_required(VERSION 3.20)

project(Settings_Project CXX)

add_library(settings STATIC
    "src/Settings.cpp"
    "src/SettingsFactory.cpp"
)

target_include_directories(settings
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
