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

/* System files */
#include <linux/if_ether.h>
#include <unistd.h>

extern "C" {
  #include "ether_comm_if.h"
  #include "can_comm_if.h"
}

#include <iostream>
#include <functional>

#include "Transmitter.hpp"
#include "CommonUtils.hpp"
#include "AvtpUtil.hpp"

#define CAN_PAYLOAD_MAX_SIZE        16*4
#define MAX_ETH_PDU_SIZE                1500

Transmitter::Transmitter(const std::string &ifname, const std::string &macaddr, const std::string &can_ifname, const std::string &channel_name)
  : m_ifname{ifname}, m_macaddr{macaddr}, m_can_ifname{can_ifname}, m_channel_name{channel_name}
{
  m_is_can_enabled = false;
  m_is_can_initialized = false;
  init();
}

Transmitter::~Transmitter()
{

}

void Transmitter::init()
{
  if(!m_is_initialized) {

    int res = -1;
    uint8_t addr[ETH_ALEN];

    res = CommonUtils::getMacAddress(m_macaddr, addr);
    if(res < 0) {
      std::cout << "Invalid mac address" << std::endl;
      return;
    }

    m_eth_fd = Comm_Ether_CreateTalkerSocket(-1);
    if (m_eth_fd < 0) {
      std::cout << "Failure to create talker socket" << std::endl;
      return;
    }

    res = Comm_Ether_SetupSocketAddress(m_eth_fd, m_ifname.c_str(), addr,
                                ETH_P_TSN, &sk_ll_addr);
    m_dest_addr = (struct sockaddr*) &sk_ll_addr;
    if (res < 0) {
      std::cout << "Failure to create destination socket address" << std::endl;
      return;
    }

    m_is_initialized = true;
    std::cout << "Talker initialized" << std::endl;
  }
}

void Transmitter::initSocketCan(bool enable)
{
  // Open a CAN socket for reading frames
  if(enable && !m_is_can_initialized) {
    m_can_reader.init(m_can_ifname, CAN_VARIANT_FD);
    m_is_can_initialized = true;
    m_is_can_enabled = true;
  }
}

void Transmitter::start()
{
  if(m_is_can_enabled && m_is_can_initialized) {
    m_running = true;
    m_transmitter_thread = std::thread{std::bind(&Transmitter::run, this)};
  }
}

void Transmitter::stop()
{
  m_running = false;

  if(m_is_can_enabled && m_is_can_initialized) {
    if (std::this_thread::get_id() != m_transmitter_thread.get_id()) {
      if (m_transmitter_thread.joinable())
      {
        m_transmitter_thread.join();
      }
    } else {
      m_transmitter_thread.detach();
    }

    m_can_reader.stop();
  }
}

void Transmitter::run()
{
  int res = 0;
  uint8_t cf_seq_num = 0;

  uint8_t pdu[MAX_ETH_PDU_SIZE];
  uint16_t pdu_length = 0;
  uint8_t num_acf_msgs = 1U;
  frame_t can_frames[num_acf_msgs];

  m_poll_fds.fd = m_can_reader.getCanSocket();
  m_poll_fds.events = POLLIN;

  while(m_running) {
    // add polling to avoid blocking calls
    res = poll(&m_poll_fds, 1, 500);
    if (res < 0) {
      std::cout <<"Failed poll() call" << std::endl;
      // destroy handler
      return;
    }

    if(!m_running) {
      std::cout << "Exit thread from talker " << std::endl;
      break;
    }

    if (res == 0) {
      // timeout expired
      continue;
    }

    if (m_poll_fds.revents & POLLIN) {
      // Read acf_num_msgs number of CAN frames from the CAN socket
      m_can_reader.receiveData(can_frames, num_acf_msgs);

      // Pack all the read frames into an AVTP frame
      pdu_length = AvtpUtil::insertCanFramesToAvtp(pdu, can_frames,
                                  num_acf_msgs, cf_seq_num++);

      // Send the packed frame out
      res = sendto(m_eth_fd, pdu, pdu_length, 0,
                    (struct sockaddr *) m_dest_addr, sizeof(struct sockaddr_ll));
      if (res < 0) {
          std::cout << "Failed to send data from talker" << std::endl;
      }
    }
  }
}

void Transmitter::sendPacket(frame_t *can_frames, uint8_t num_acf_msgs)
{
  int res = 0;
  uint8_t cf_seq_num = 0;
  uint8_t pdu[MAX_ETH_PDU_SIZE];
  uint16_t pdu_length = 0;

  // Pack all the read frames into an AVTP frame
  pdu_length = AvtpUtil::insertCanFramesToAvtp(pdu, can_frames,
                              num_acf_msgs, cf_seq_num++);

  // Send the packed frame out
  res = sendto(m_eth_fd, pdu, pdu_length, 0,
                (struct sockaddr *) m_dest_addr, sizeof(struct sockaddr_ll));
  if (res < 0) {
      std::cout << "Failed to send data from talker" << std::endl;
  }
}