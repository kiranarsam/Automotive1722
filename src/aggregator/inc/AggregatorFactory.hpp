
#pragma once

#include "IAggregator.hpp"
#include <memory>

class AggregatorFactory final
{
private:
  AggregatorFactory() = delete;
  ~AggregatorFactory() = delete;
public:
  static std::shared_ptr<IAggregator> createCanAggregator();
};