
#include "DataCallbackHandler.hpp"
#include <iostream>

DataCallbackHandler::DataCallbackHandler()
{

}

DataCallbackHandler::~DataCallbackHandler()
{
  std::cout << "~DataCallbackHandler" << std::endl;
}

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

void DataCallbackHandler::unRegisterCallback()
{
  m_callback = {};
}
