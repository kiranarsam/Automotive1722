
#pragma once

#include <functional>
#include <string>

#include "IeeeCanCommon.hpp"

typedef struct {
  std::string name;
  CanFrame cf;
} callback_data;

typedef std::function<void(callback_data &)> canCallbackHandler;

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