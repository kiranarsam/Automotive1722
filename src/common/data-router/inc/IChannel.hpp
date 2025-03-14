#pragma once

#include "DataCallbackHandler.hpp"
#include "IeeeCanCommon.hpp"

class IChannel
{
public:
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void registerCallbackHandler(DataCallbackHandler &&handler) = 0;
  virtual void unRegisterCallbackHandler() = 0;
  virtual void sendFrames(frame_t *frames, uint8_t num_msgs) = 0;
};