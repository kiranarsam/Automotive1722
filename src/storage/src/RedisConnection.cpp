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

#include "RedisConnection.hpp"
#include <iostream>
#include <sstream>

RedisConnection::RedisConnection(RedisConnectionCallback callback, ResponseCallbackMsg res_callback)
    : m_callback{callback}, m_res_callback{res_callback}, m_is_running{false}, m_is_cmd_running{false}, m_context{nullptr}
{
  connectRedis();
}

RedisConnection::~RedisConnection()
{

}

void RedisConnection::connectRedis()
{
  static uint32_t iteration_count = 0;
  redisOptions options = {0};
  REDIS_OPTIONS_SET_TCP(&options, "127.0.0.1", 6379);
  m_context = redisConnectWithOptions(&options);

  if(!m_context) {
    std::cout << "Redis Unable to create a context" << std::endl;
    return;
  }

  if (m_context->err) {
      std::cout << "Redis Error: " << m_context->errstr << "\n" << std::endl;
      redisFree(m_context);
      m_context = nullptr;
      if((iteration_count/10) > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
      sendRedisConnectionStatus(RedisStateType::NOT_CONNECTED);
  } else {
      iteration_count = 0;
      sendRedisConnectionStatus(RedisStateType::CONNECTED);
  }
  iteration_count++;
}

void RedisConnection::startRedis()
{
  if(!m_is_running && m_context) {
    m_is_running = true;
    m_thread = std::thread{std::bind(&RedisConnection::run, this)};
  }
}

void RedisConnection::stopRedis()
{
  if(m_is_running) {
    m_is_running = false;
    if (std::this_thread::get_id() != m_thread.get_id()) {
      if (m_thread.joinable()) {
        m_thread.join();
      } else {
        m_thread.detach();
      }
    }
  }
  redisFree(m_context);
  m_context = nullptr;
}

void RedisConnection::start()
{
  startRedis();

  if(!m_is_cmd_running) {
    m_is_cmd_running = true;
    m_cmd_thread = std::thread{std::bind(&RedisConnection::runCommand, this)};
  }
}

void RedisConnection::stop()
{
  stopRedis();

  if(m_is_cmd_running) {
    m_is_cmd_running = false;
    m_cmd_cond_var.notify_one();

    if (std::this_thread::get_id() != m_cmd_thread.get_id()) {
      if (m_cmd_thread.joinable()) {
        m_cmd_thread.join();
      } else {
        m_cmd_thread.detach();
      }
    }

    while(!m_cmd_queue.empty()) {
      m_cmd_queue.pop();
    }
  }
}

void RedisConnection::run() {
  static uint32_t iteration_count = 0;
  while(m_is_running) {
    bool re_connect = true;
    if(!m_is_running) {
      std::cout << "Exited from redis connection thread" << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    redisReply* reply = nullptr;
    if(m_context) {
      reply = (redisReply*)redisCommand(m_context, "PING");
    }

    if(reply != nullptr) {
      if ((reply->type == REDIS_REPLY_STATUS) && (strcmp(reply->str, "PONG") == 0)) {
        re_connect = false;
        iteration_count = 0;
      }
    }

    freeReplyObject(reply);

    if(re_connect) {
      if((iteration_count / 10) > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
      sendRedisConnectionStatus(RedisStateType::NOT_DISCONNECTED);
    }
    iteration_count++;
  }
}

void RedisConnection::runCommand()
{
  while(m_is_cmd_running) {
    std::unique_lock<std::mutex> lock(m_cmd_mutex);
    m_cmd_cond_var.wait(lock, [this] { return !m_cmd_queue.empty() || !m_is_cmd_running; });

    if(!m_is_cmd_running) {
      std::cout << "Exited from process command thread " << std::endl;
      break;
    }

    if(m_state == RedisStateType::CONNECTED) {
      if(!m_cmd_queue.empty()) {
        auto& res = m_cmd_queue.front();
        //process
        processCommand(res);
        m_cmd_queue.pop();
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}

void RedisConnection::processCommand(const RedisRequestMsg& req)
{
  RedisResponseMsg res;
  res.type = req.type;
  res.key = req.key;
  res.reply = {};
  std::stringstream query;
  if(req.type == RedisDbType::SET) {
    query << "SET" << " " << req.key << " " << req.value;
  }
  else if(req.type == RedisDbType::GET) {
    query << "GET" << " " << req.key;
  }
  redisReply *reply = (redisReply *)redisCommand(m_context, query.str().c_str());
  if(reply != nullptr) {
    // std::cout << "REPLY: " << reply->str << std::endl;
    res.reply = std::string{reply->str};
  }

  freeReplyObject(reply);
  if(m_res_callback) {
    m_res_callback(std::move(res));
  }
}

void RedisConnection::command(const RedisRequestMsg& req)
{
  std::unique_lock<std::mutex> lock(m_cmd_mutex);
  m_cmd_queue.push(std::move(req));
  m_cmd_cond_var.notify_one();
}

void RedisConnection::sendRedisConnectionStatus(RedisStateType state)
{
  m_state = state;
  if(m_callback) {
    m_callback(m_state);
  }
}