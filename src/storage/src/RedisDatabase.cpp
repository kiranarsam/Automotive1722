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
