
#pragma once

#include "IAggregator.hpp"
#include "DataCallbackHandler.hpp"
#include "DataRouter.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Aggregator : public IAggregator
{
  public:
    Aggregator();
    ~Aggregator();

    void init();
    void start();
    void stop();

    void setDataHandler(DataCallbackHandler &&handler);
    void createDataHandlers();

  protected:
    std::mutex m_mutex;
    std::condition_variable m_cond_var;

    std::queue<callback_data> m_queue;
    DataRouter m_data_router;
    DataCallbackHandler m_handler;

  private:
    void run();
    bool m_is_initialized;
    bool m_is_running;
    std::thread m_thread;
};