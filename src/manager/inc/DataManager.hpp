#pragma once

#include "IAggregator.hpp"
#include "IDataSubscriber.hpp"

#include <vector>

class DataManager : public IDataSubscriber
{
  public:
    DataManager();
    ~DataManager();
    void init();
    void start();
    void stop();
    void registerDataObserver(std::shared_ptr<IDataObserver> data_observer);
    void unRegisterDataObserver(std::shared_ptr<IDataObserver> data_observer);
  private:

    void notify();

    bool m_is_initialized;
    std::shared_ptr<IAggregator> m_can_aggregator;
    std::vector<std::shared_ptr<IDataObserver>> m_observers;
};