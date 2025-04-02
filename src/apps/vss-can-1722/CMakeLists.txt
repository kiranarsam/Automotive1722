project(VssDataExchange_Project)

add_library(vsscanexchange STATIC
  "src/VssCanExchange.cpp"
)

target_include_directories(vsscanexchange
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(vsscanexchange
  datamanager
  agents
)

add_executable(app_vss_can_1722
  "src/main.cpp"
)

target_include_directories(app_vss_can_1722
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(app_vss_can_1722
  vsscanexchange
)

install(TARGETS
  app_vss_can_1722
  DESTINATION "${CMAKE_INSTALL_PREFIX}"
)