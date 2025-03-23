#pragma once

class IDataObserver
{
public:
  virtual void update() = 0;

  virtual ~IDataObserver() {

  }
};