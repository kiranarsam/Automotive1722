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

#include "DataManager.hpp"
#include "AgentFactory.hpp"

#include <mutex>
#include <condition_variable>
#include <csignal>
#include <iostream>

std::mutex g_mutex;
std::condition_variable g_cond_var;

bool g_ready = false;

void handleSignal(int sig);

void handleSignal(int sig)
{
  if((sig == SIGINT) || (sig == SIGTERM)) {
    std::cout << "EXITED invoked sigint or sigterm " << std::endl;
    g_ready = true;
    g_cond_var.notify_one();
  }
}

int main() {
  // Data Manager
  DataManager dm;
  // Can Agent
  auto agent = AgentFactory::createCanAgent();

  dm.registerDataObserver(agent);
  agent->start();
  dm.start();

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  std::unique_lock<std::mutex> lock(g_mutex);
  g_cond_var.wait(lock, [&] { return g_ready; });
  lock.unlock();

  //dm.unRegisterDataObserver(agent);
  // Stop the agent
  // Stop data Manager
  dm.stop();
  agent->stop();

  return 0;
}