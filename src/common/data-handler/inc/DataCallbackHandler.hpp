
#pragma once

#include "IeeeCanCommon.hpp"
#include <string>
#include <functional>

typedef struct {
  std::string name;
  frame_t can_data;
} callback_data;

typedef std::function<void (callback_data &)> canCallbackHandler;

class DataCallbackHandler
{
public:
  DataCallbackHandler();
  ~DataCallbackHandler();
  void registerCallback(canCallbackHandler callback);

  void handleCallback(callback_data &msg);

  void unRegisterCallback();

private:
  canCallbackHandler m_callback;
};