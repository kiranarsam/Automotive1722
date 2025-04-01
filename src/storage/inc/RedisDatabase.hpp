#pragma once

#include "RedisConnection.hpp"

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

class RedisDatabase
{
public:
  RedisDatabase();
  ~RedisDatabase();

  void init();

  void start();

  void stop();

  void command(const RedisRequestMsg &req);

private:

  void run();

  void connectionCallback(RedisStateType state);

  void responseCallbackMessage(RedisResponseMsg &&res);

  void reConnect();

  std::shared_ptr<RedisConnection> m_redis_conn;

  bool m_is_running;
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cond_var;

  RedisStateType m_state;
  bool m_ready;

};