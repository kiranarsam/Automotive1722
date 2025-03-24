#include "DataManager.hpp"
#include "AggregatorFactory.hpp"
#include "ProcessCanMessage.hpp"

extern "C" {
  #include "can_codec.h"
}

#include <iostream>

DataManager::DataManager() : m_is_initialized{}, m_can_aggregator{nullptr}, m_observers{}, m_queue{}
{
  init();
}

DataManager::~DataManager()
{

}

void DataManager::init()
{
  if(!m_is_initialized) {

    CanDbc canDbc{"dbc.dbc", dbcCache};

    auto callback = [this](callback_data &msg) {
      std::cout << "DataManager data = " << msg.name << std::endl;
      std::cout << "DataManager data can_type = " << static_cast<int>(msg.cf.type) << std::endl;
      std::cout << "DataManager data can_id = " << msg.cf.data.cc.can_id << std::endl;

      {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_queue.push(std::move(msg));
        m_cond_var.notify_one();
      }
    };

    DataCallbackHandler handler;
    handler.registerCallback(callback);

    m_can_aggregator = AggregatorFactory::createCanAggregator();

    m_can_aggregator->setDataHandler(std::move(handler));

    m_is_initialized = true;
  }
}

void DataManager::run() {
  while (m_is_running) {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_cond_var.wait(lock, [this] { return !m_queue.empty() || !m_is_running; });
    if(!m_is_running) {
      std::cout << "Data Manager thread exited " << std::endl;
      break;
    }

    if(!m_queue.empty()) {
      auto& msg = m_queue.front();
      lock.unlock();

      // Process the message
      processReceivedMessage(msg);

      lock.lock();
      m_queue.pop();
      lock.unlock();
    }
  }
}

void DataManager::processReceivedMessage(callback_data &msg)
{
  // notify to agents
  CanMessage can_msg;
  constructMessage(msg, can_msg);

  // forward the data can_msg
  notify();
}

void DataManager::constructMessage(callback_data &msg, CanMessage &can_msg)
{
  can_msg.type = static_cast<int>(msg.cf.type);
  if(can_msg.type == 1) {
    can_msg.can_id = msg.cf.data.fd.can_id;
  } else {
    can_msg.can_id = msg.cf.data.cc.can_id;
  }

  try {
    auto& dbc_msg = dbcCache.at(can_msg.can_id);
    if(can_msg.type == 1) {
      ProcessCanMessage::process(msg.cf.data.fd.data, dbc_msg, can_msg);
    } else {
      ProcessCanMessage::process(msg.cf.data.cc.data, dbc_msg, can_msg);
    }
  } catch(...) {
    std::cout << "Unknown can frame received: " << can_msg.can_id << std::endl;
  }
}

void DataManager::start()
{
  if(m_is_initialized) {
    m_is_running = true;
    m_thread = std::thread{std::bind(&DataManager::run, this)};

    if(m_can_aggregator) {
      m_can_aggregator->start();
    }
  }
}

void DataManager::stop()
{
  m_is_running = false;
  m_cond_var.notify_one();
  if (std::this_thread::get_id() != m_thread.get_id()) {
    if (m_thread.joinable()) {
      m_thread.join();
    } else {
      m_thread.detach();
    }
  }

  while (!m_queue.empty()) {
    m_queue.pop();
  }

  if(m_can_aggregator) {
    m_can_aggregator->stop();
  }
  // clear all the observers
  m_observers.clear();
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
