
#pragma once

#include <functional>

class DataCallbackHandler
{
public:
  DataCallbackHandler();
  ~DataCallbackHandler();
  void registerCallback(std::function<void(int)> callback);

  void handleCallback(int);

  void unRegisterCallback();

private:
  std::function<void(int)> m_callback;
};