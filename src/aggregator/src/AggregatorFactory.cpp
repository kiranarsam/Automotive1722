
#include "AggregatorFactory.hpp"
#include "CanAggregator.hpp"

std::shared_ptr<IAggregator> AggregatorFactory::createCanAggregator()
{
  return std::make_shared<CanAggregator>();
}