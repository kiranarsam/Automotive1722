project(Aggregator_Project)

add_library(aggregator STATIC
  "src/Aggregator.cpp"
  "src/Aggregator.hpp"
  "src/CanAggregator.cpp"
  "src/CanAggregator.hpp"
  "src/AggregatorFactory.cpp"
)

target_include_directories(aggregator
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(aggregator
  datarouter
  datahandler
)
