#pragma once

#include "IDataObserver.hpp"
#include <memory>

class IDataSubscriber
{
public:
  virtual void registerDataObserver(std::shared_ptr<IDataObserver> data_observer) = 0;
  virtual void unRegisterDataObserver(std::shared_ptr<IDataObserver> data_observer) = 0;

  virtual ~IDataSubscriber() {

  }
};