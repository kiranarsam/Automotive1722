
#include "DataCallbackHandler.hpp"

void DataCallbackHandler::registerCallback(std::function<void(int)> callback)
{
  m_callback = callback;
}

void DataCallbackHandler::handleCallback(int data)
{
  if(m_callback) {
    m_callback(data);
  }
}
