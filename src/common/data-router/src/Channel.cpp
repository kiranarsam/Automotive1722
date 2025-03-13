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

#include "Channel.hpp"

Channel::Channel(std::string &ifname, std::string &macaddr)
  : m_ifname{ifname}, m_macaddr{macaddr}, m_receiver{nullptr}, m_transmitter{nullptr}, m_is_initialized{false}
{
  init();
}

Channel::~Channel()
{

}

void Channel::init()
{
  if(!m_is_initialized) {
    m_receiver = std::make_shared<Receiver>(m_ifname, m_macaddr);
    m_transmitter = std::make_shared<Transmitter>(m_ifname, m_macaddr);

    m_is_initialized = true;
  }
}

void Channel::setCallbackHandler(DataCallbackHandler &handler)
{
  m_receiver->setCallbackHandler(handler);
}

void Channel::start()
{
  m_receiver->start();
  m_transmitter->start();
}

void Channel::stop()
{
  m_is_initialized = false;
  m_receiver->stop();
  m_transmitter->stop();
}
