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

#include "CanAgent.hpp"
#include <iostream>
#include <functional>

CanAgent::CanAgent() : m_is_running{false}, m_redis{nullptr}, m_vss_mapper{nullptr}
{
  init();
}

CanAgent::~CanAgent()
{

}

void CanAgent::init()
{
  m_redis = std::make_shared<RedisDatabase>();
  m_vss_mapper = std::make_shared<VssMapper>();
}

void CanAgent::update(std::shared_ptr<CanMessage> can_msg)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_queue.push(can_msg);
  m_cond_var.notify_one();
}

void CanAgent::start()
{
  if(!m_is_running) {
    m_is_running = true;

    m_redis->start();

    m_thread = std::thread{std::bind(&CanAgent::run, this)};
  }
}

void CanAgent::stop()
{
  if(m_is_running) {
    m_is_running = false;
    m_cond_var.notify_one();

    if (std::this_thread::get_id() != m_thread.get_id()) {
      if (m_thread.joinable()) {
        m_thread.join();
      } else {
        m_thread.detach();
      }
    }

    while(!m_queue.empty()) {
      m_queue.pop();
    }

    m_redis->stop();
  }
}

void CanAgent::run()
{
  while(m_is_running) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond_var.wait(lock, [this] { return !m_queue.empty() || !m_is_running; });

    if(!m_is_running) {
      std::cout << "Exited from Can Agent thread " << std::endl;
      break;
    }

    if(!m_queue.empty()) {
      auto msg = m_queue.front();
      m_queue.pop();
      lock.unlock();
      processMessage(msg);
    }
  }
}

void CanAgent::processMessage(std::shared_ptr<CanMessage> can_msg)
{
  std::cout << "CanAgent received can_id: " <<  can_msg->can_id << std::endl;
  std::cout << "CanAgent signals size: " <<  can_msg->signals.size() << std::endl;
  for(auto& signal : can_msg->signals) {
    std::string vss_name {};
    m_vss_mapper->getVssNameFromDbcMap(signal.name, vss_name);
    if(!vss_name.empty()) {
      RedisRequestMsg req;
      req.type = RedisDbType::SET;
      req.key = vss_name;
      req.value = std::to_string(signal.value);
      std::cout << "vss_name : " << vss_name << ", value: " << signal.value << std::endl;
      m_redis->command(req);
    }
  }
}