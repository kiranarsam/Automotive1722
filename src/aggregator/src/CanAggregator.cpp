

#include "CanAggregator.hpp"
#include "SettingsFactory.hpp"
#include <iostream>

CanAggregator::CanAggregator()
{
  std::cout << "CanAggregator" << std::endl;
}

CanAggregator::~CanAggregator()
{
  std::cout << "~CanAggregator" << std::endl;
}

void CanAggregator::setDataHandler(DataCallbackHandler &&handler)
{
  std::cout << "CanAggregator setDataHandler() " << std::endl;
  m_handler = std::move(handler);
}
