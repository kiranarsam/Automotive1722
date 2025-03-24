#pragma once

#include "IAggregator.hpp"
#include "IDataSubscriber.hpp"
#include "CanDbc.hpp"

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

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

    void run();

    void processReceivedMessage(callback_data &msg);
    void constructMessage(callback_data &msg, CanMessage &can_msg);
    //void processTransmittedMessage()

    bool m_is_initialized;
    std::shared_ptr<IAggregator> m_can_aggregator;
    std::vector<std::shared_ptr<IDataObserver>> m_observers;

    std::queue<callback_data> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
    std::thread m_thread;

    bool m_is_running;
    std::unordered_map<uint32_t, DbCanMessage> dbcCache;
};