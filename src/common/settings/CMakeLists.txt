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

add_executable(test_settings
  "src/test_settings.cpp"
)

target_include_directories(test_settings
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(test_settings
  settings
)

install(FILES
    ./config/settings.dat
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(TARGETS
    test_settings
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
