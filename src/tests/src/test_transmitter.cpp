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

#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <mutex>
#include <thread>

#include "Transmitter.hpp"

std::mutex g_mutex;
std::condition_variable g_cond_var;

bool g_ready = false;
bool g_running = true;

void handleSignal(int sig);

void handleSignal(int sig)
{
  if ((sig == SIGINT) || (sig == SIGTERM)) {
    g_ready = true;
    g_running = false;
    g_cond_var.notify_one();
  }
}

CanFrame can_frames[2];

int main()
{
  std::string ifname{"ens160"};
  std::string macaddr{"00:50:56:b0:74:a4"};
  std::string can_ifname{""};
  Transmitter transmitter{"lo", "00:50:56:b0:74:a4", "", ""};
  transmitter.init();
  transmitter.start();

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  CanFrame *frame1 = &can_frames[0];
  CanFrame *frame2 = &can_frames[1];
  frame1->type = CanVariant::CAN_VARIANT_CC;
  frame1->data.cc.can_id = 0x06F;
  frame1->data.cc.len = 8;
  frame2->type = CanVariant::CAN_VARIANT_CC;
  frame2->data.cc.can_id = 0x0DE;
  frame2->data.cc.len = 8;
  uint8_t payload1[8] = {0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  uint8_t payload2[8] = {0x10, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
  memcpy(frame1->data.cc.data, payload1, 8);
  memcpy(frame2->data.cc.data, payload2, 8);

  while (g_running) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    if (!g_running) {
      break;
    }

    transmitter.sendPacket(can_frames, 2U);
  }

  std::unique_lock<std::mutex> lock(g_mutex);
  g_cond_var.wait(lock, [&] { return g_ready; });
  lock.unlock();

  transmitter.stop();

  return 0;
}