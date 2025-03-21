
#include "Aggregator.hpp"

#include <functional>
#include <iostream>

Aggregator::Aggregator() : m_is_initialized{false}, m_mutex{}, m_cond_var{},
  m_is_running{false}, m_thread{}
{
  std::cout << "Aggregator" << std::endl;
  init();
}

Aggregator::~Aggregator()
{
  std::cout << "~Aggregator" << std::endl;
}

void Aggregator::init()
{
  std::cout << "Aggregator init() " << std::endl;
  if(!m_is_initialized) {
    createDataHandlers();
    m_is_initialized = true;
  }
}

void Aggregator::start()
{
  if (m_is_initialized) {
    m_is_running = true;
    m_thread = std::thread{std::bind(&Aggregator::run, this)};

    m_data_router.start();
  }
}

void Aggregator::setDataHandler(DataCallbackHandler &&handler)
{
  std::cout << "Aggregator setDataHandler" << std::endl;
}

void Aggregator::createDataHandlers()
{
  std::cout << "Aggregator createDataHandlers" << std::endl;
  std::vector<std::string> channel_list {};
  m_data_router.getListOfChannelNames(channel_list);
  std::cout << "Aggregator createDataHandlers 11" << std::endl;
  for (auto &channel_name : channel_list)
  {
    std::cout << "Channel Name: " << channel_name << std::endl;
    auto callback = [this](callback_data &msg) {
      {
        std::cout << "Aggregator: received data: " << msg.name << std::endl;
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(std::move(msg));
        m_cond_var.notify_one();
      }
    };

    DataCallbackHandler handler;
    handler.registerCallback(callback);

    m_data_router.registerDataCallbackHandler(channel_name, std::move(handler));
  }
}

void Aggregator::stop()
{
  if(!m_is_initialized) {
    return;
  }
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

  m_data_router.stop();
}

void Aggregator::run()
{
  while(m_is_running)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_cond_var.wait(lock, [this]() { return !m_queue.empty() || !m_is_running; });

    if (!m_is_running)
    {
      std::cout << "Exited thread" << std::endl;
      break;
    }
    // retreive data
    auto &msg = m_queue.front();
    lock.unlock();
    // process over here
    //std::cout << "Data = " << msg.name << std::endl;
    m_handler.handleCallback(msg);

    lock.lock();
    m_queue.pop();
    lock.unlock();
  }
}