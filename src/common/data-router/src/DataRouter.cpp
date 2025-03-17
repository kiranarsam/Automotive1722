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
#include "SettingsFactory.hpp"

#include <iostream>
#include <sstream>

DataRouter::DataRouter() : m_is_initialized{false}, m_channels{}
{
  init();
}

DataRouter::~DataRouter()
{

}

void DataRouter::init()
{
  if(!m_is_initialized) {
    auto settings = SettingsFactory::GetInstance();
    std::stringstream ss; // convert for specific data types
    uint8_t num_channels;
    auto num_channels_str = settings->getData(SettingsKey::SECTION_DEFAULT, SettingsKey::NUM_CHANNEL);
    ss << num_channels_str;
    ss >> num_channels;
    for (uint8_t section = 1; section <= num_channels; section++)
    {
      std::stringstream index_ss;
      index_ss << section;
      // consider channel name is same as section name
      auto channel_name = SettingsKey::SECTION_CHANNEL + index_ss.str();
      auto ifname = settings->getData(channel_name, SettingsKey::ETH_IF_NAME);
      auto macaddr = settings->getData(channel_name, SettingsKey::DEST_MAC_ADDR);
      auto can_reciever = settings->getData(channel_name, SettingsKey::CAN_IF_NAME_RECEIVER);
      auto can_transmitter = settings->getData(channel_name, SettingsKey::CAN_IF_NAME_TRANSMITTER);
      auto enable_can_receiver = settings->getData(channel_name, SettingsKey::CAN_RECEIVER_ENABLED);
      auto enable_can_transmitter = settings->getData(channel_name, SettingsKey::CAN_TRANSMITTER_ENABLED);

      std::shared_ptr<IChannel> channel = std::make_shared<Channel>(ifname, macaddr, can_reciever, can_transmitter, channel_name);

      m_channels[channel_name] = channel;

      if(enable_can_receiver.compare("true") == 0) {
        channel->allowVirtualCanForReceiver(true);
      }
      if(enable_can_transmitter.compare("true") == 0) {
        channel->allowVirtualCanForTransmitter(true);
      }
    }

    m_is_initialized = true;
  }
}

void DataRouter::registerDataCallbackHandler(const std::string &channel_name, DataCallbackHandler &&handler)
{
  try {
    auto channel = m_channels.at(channel_name);
    channel->registerCallbackHandler(std::move(handler));
  }
  catch(...) {
    std::cout << "Unable to register with channel: " << channel_name << std::endl;
  }
}

void DataRouter::unRegisterDataCallbackHandler(const std::string &channel_name)
{
  try {
    auto channel = m_channels.at(channel_name);
    channel->unRegisterCallbackHandler();
  }
  catch(...) {
    std::cout << "Unable to unregister with channel: " << channel_name << std::endl;
  }
}

void DataRouter::publishFrames(const std::string &channel_name, frame_t *frames, uint8_t num_msgs)
{
  try {
    auto channel = m_channels.at(channel_name);
    channel->sendFrames(frames, num_msgs);
  }
  catch(...) {
    std::cout << "Unable to send frames on channel: " << channel_name << std::endl;
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