
#pragma once

#include "DataCallbackHandler.hpp"

class IAggregator
{
  public:
    virtual void init() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void setDataHandler(DataCallbackHandler &&handler) = 0;
    virtual void createDataHandlers() = 0;

    virtual ~IAggregator() {}
};