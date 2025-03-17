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

/* System files */
#include <poll.h>

#include <string>
#include <thread>

#include "CanWriter.hpp"
#include "DataCallbackHandler.hpp"

class Receiver
{
public:

  Receiver(const std::string &ifname, const std::string &macaddr, const std::string &can_ifname);

  ~Receiver();

  void init();

  void initSocketCan(bool enable);

  void registerCallbackHandler(DataCallbackHandler &&handler);

  void unRegisterCallbackHandler();

  void start();

  void stop();

private:

  void run();

  std::string m_ifname;

  std::string m_macaddr;

  std::string m_can_ifname;

  int m_eth_fd;

  bool m_is_initialized;

  bool m_is_can_enabled;

  std::thread m_receiver_thread;

  bool m_running;

  struct pollfd m_poll_fds;

  bool m_is_can_initialized;

  CanWriter m_can_writer;

  DataCallbackHandler m_callback_handler;
};