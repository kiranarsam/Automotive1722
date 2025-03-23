#include "DataManager.hpp"
#include "AggregatorFactory.hpp"

#include <iostream>

DataManager::DataManager() : m_is_initialized{}, m_can_aggregator{nullptr}, m_observers{}
{
  init();
}

DataManager::~DataManager()
{

}

void DataManager::init()
{
  if(!m_is_initialized) {

    auto callback = [this](callback_data &msg) {
      std::cout << "DataManager data = " << msg.name << std::endl;
      std::cout << "DataManager data can_type = " << static_cast<int>(msg.cf.type) << std::endl;
      std::cout << "DataManager data can_id = " << msg.cf.data.cc.can_id << std::endl;

      // notify to agents
      notify();
    };

    DataCallbackHandler handler;
    handler.registerCallback(callback);

    m_can_aggregator = AggregatorFactory::createCanAggregator();

    m_can_aggregator->setDataHandler(std::move(handler));

    m_is_initialized = true;
  }
}

void DataManager::start()
{
  if(m_can_aggregator) {
    m_can_aggregator->start();
  }
}

void DataManager::stop()
{
  if(m_can_aggregator) {
    m_can_aggregator->stop();
  }
}

void DataManager::registerDataObserver(std::shared_ptr<IDataObserver> data_observer)
{
  if(data_observer) {
    m_observers.push_back(data_observer);
  }
}

void DataManager::unRegisterDataObserver(std::shared_ptr<IDataObserver> data_observer)
{
  if (data_observer) {
    for (auto it = m_observers.begin(); it != m_observers.end(); it++) {
      auto& to_delete = *it;
      if(to_delete.get() == data_observer.get()) {
        std::cout << "Removed the observer from data manager" << std::endl;
        m_observers.erase(it);
        break;
      }
    }
  }
}

void DataManager::notify()
{
  for(auto items : m_observers) {
    items->update();
  }
}
