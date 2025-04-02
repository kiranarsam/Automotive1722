/**
 * Copyright (c) 2025, Kiran Kumar Arsam
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *   * this list of conditions and the following disclaimer in the documentation
 *   * and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Kiran Kumar Arsam nor the names of its
 *   * contributors may be used to endorse or promote products derived from
 *   * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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