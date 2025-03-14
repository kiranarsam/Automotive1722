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

#include "DataRouter.hpp"
#include "Channel.hpp"

DataRouter::DataRouter() : m_channels{}, m_is_initialized{false}
{
  init();
}

DataRouter::~DataRouter()
{

}

void DataRouter::init()
{
  if(!m_is_initialized) {
    // example
    std::string ifname = "eth0";
    std::string macaddr = "aa::bb::cc:dd:ee:ff";
    // create number of channels
    m_channels["CHAN01"] = std::make_shared<Channel>(ifname, macaddr);

    m_is_initialized = true;
  }
}

void DataRouter::registerDataCallbackHandler(const std::string &channel_name, DataCallbackHandler &&handler)
{
  auto it = m_channels.find(channel_name);
  if(it != m_channels.end()) {
    auto channel = it->second;
    channel->registerCallbackHandler(std::move(handler));
  }
}

void DataRouter::unRegisterDataCallbackHandler(const std::string &channel_name)
{
  auto it = m_channels.find(channel_name);
  if(it != m_channels.end()) {
    auto channel = it->second;
    channel->unRegisterCallbackHandler();
  }
}

void DataRouter::publishFrames(const std::string &channel_name, frame_t *frames, uint8_t num_msgs)
{
  auto it = m_channels.find(channel_name);
  if(it != m_channels.end()) {
    auto channel = it->second;
    channel->sendFrames(frames, num_msgs);
  }
}

void DataRouter::start()
{
  for (auto &pair : m_channels) {
    auto channel = pair.second;
    channel->start();
  }
}

void DataRouter::stop()
{
  for (auto &pair : m_channels) {
    auto channel = pair.second;
    channel->stop();
  }

  m_channels.clear();
}