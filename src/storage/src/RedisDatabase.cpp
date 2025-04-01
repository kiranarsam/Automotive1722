
#include "RedisDatabase.hpp"
#include <iostream>
#include <functional>
#include <chrono>

RedisDatabase::RedisDatabase() : m_redis_conn{nullptr}, m_is_running{false}, m_ready{false}
{
  init();
}

RedisDatabase::~RedisDatabase() {

}

void RedisDatabase::connectionCallback(RedisStateType state)
{
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    // notify to start connection again
    m_state = state;
    m_ready = true;
    m_cond_var.notify_one();
  }
}

void RedisDatabase::responseCallbackMessage(RedisResponseMsg &&res)
{
  std::cout << "Data received: " << res.reply << std::endl;
}

void RedisDatabase::init()
{
  m_redis_conn = std::make_shared<RedisConnection>(
    std::bind(&RedisDatabase::connectionCallback, this, std::placeholders::_1),
    std::bind(&RedisDatabase::responseCallbackMessage, this, std::placeholders::_1)
  );
}

void RedisDatabase::start()
{
  m_is_running = true;
  m_thread = std::thread{std::bind(&RedisDatabase::run, this)};
  m_redis_conn->start();
}

void RedisDatabase::stop()
{
  m_state = RedisStateType::CLOSED;
  m_is_running = false;
  m_ready = true;
  m_cond_var.notify_one();

  if (std::this_thread::get_id() != m_thread.get_id()) {
    if (m_thread.joinable()) {
      m_thread.join();
    } else {
      m_thread.detach();
    }
  }

  m_redis_conn->stop();
}

void RedisDatabase::run()
{
  while(m_is_running) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond_var.wait(lock, [this] { return m_ready; });

    if(!m_is_running) {
      std::cout << "Exited Db repository thread" << std::endl;
      break;
    }
    m_ready = false;
    lock.unlock();
    reConnect();
  }
}

void RedisDatabase::reConnect()
{
  if(m_state == RedisStateType::CLOSED) {
    std::cout << "Closing Database Repo" << std::endl;
  } else if(m_state == RedisStateType::NOT_CONNECTED || m_state == RedisStateType::NOT_DISCONNECTED) {
    m_redis_conn->stopRedis();
    m_redis_conn->connectRedis();
    m_redis_conn->startRedis();
  }
}

void RedisDatabase::command(const RedisRequestMsg &req)
{
  if(m_redis_conn) {
    m_redis_conn->command(req);
  }
}
