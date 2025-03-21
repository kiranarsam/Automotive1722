
#include "DataCallbackHandler.hpp"
#include <iostream>

DataCallbackHandler::DataCallbackHandler()
{

}

DataCallbackHandler::~DataCallbackHandler()
{
  std::cout << "~DataCallbackHandler" << std::endl;
}

void DataCallbackHandler::registerCallback(canCallbackHandler callback)
{
  m_callback = callback;
}

void DataCallbackHandler::handleCallback(callback_data &msg)
{
  if(m_callback) {
    m_callback(msg);
  }
}

void DataCallbackHandler::unRegisterCallback()
{
  m_callback = {};
}
