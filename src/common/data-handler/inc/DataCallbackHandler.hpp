
#pragma once

#include <functional>

class DataCallbackHandler
{
public:
  void registerCallback(std::function<void(int)> callback);

  void handleCallback(int);

private:
  std::function<void(int)> m_callback;
};