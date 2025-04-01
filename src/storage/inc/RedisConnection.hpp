#pragma once

#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>

extern "C" {
#include "hiredis.h"
// #include "async.h"
#include "adapters/poll.h"
}

enum class RedisStateType {
  INIT,
  CONNECTED,
  NOT_CONNECTED,
  DISCONNECTED,
  NOT_DISCONNECTED,
  CLOSED
};

enum class RedisDbType {
  SET,
  GET
};

typedef struct RedisRequestMsg {
  RedisDbType type;
  std::string key;
  std::string value;
} RedisRequestMsg;

typedef struct RedisResponseMsg {
  RedisDbType type;
  std::string key;
  std::string reply;
}RedisResponseMsg;

typedef std::function<void(RedisStateType)> RedisConnectionCallback;
typedef std::function<void(RedisResponseMsg &&res)> ResponseCallbackMsg;

class RedisConnection
{
public:
  RedisConnection(RedisConnectionCallback callback, ResponseCallbackMsg res_callback);
  ~RedisConnection();

  void start();

  void stop();

  void startRedis();

  void stopRedis();

  void command(const RedisRequestMsg& req);

  void connectRedis();

  void sendRedisConnectionStatus(RedisStateType state);

private:

  void run();

  void runCommand();

  void processCommand(const RedisRequestMsg& req);

  RedisConnectionCallback m_callback;
  ResponseCallbackMsg m_res_callback;
  bool m_is_running;
  std::thread m_thread;
  redisContext *m_context;

  std::mutex m_mutex;
  std::condition_variable m_cond_var;

  bool m_is_cmd_running;
  std::thread m_cmd_thread;
  std::mutex m_cmd_mutex;
  std::condition_variable m_cmd_cond_var;
  std::queue<RedisRequestMsg> m_cmd_queue;

  RedisStateType m_state;
};