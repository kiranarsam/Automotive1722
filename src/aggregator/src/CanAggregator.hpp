
#pragma once

#include "Aggregator.hpp"

class CanAggregator : public Aggregator
{
  public:
    CanAggregator();
    ~CanAggregator();
    void setDataHandler(DataCallbackHandler &&handler);
};