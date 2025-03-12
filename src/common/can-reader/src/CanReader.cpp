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

#include "CanReader.hpp"

/* System header files */
#include <unistd.h>

extern "C" {
  #include "can_comm_if.h"
}

#include <iostream>

CanReader::CanReader()
  : m_ifname{}, m_can_variant{}, m_can_socket{-1}, m_is_initialized{false}
{

}

CanReader::~CanReader()
{

}

void CanReader::init(std::string &ifname, uint8_t can_variant)
{
  if (!m_is_initialized)
  {
    m_ifname = ifname;
    m_can_variant = can_variant;

    m_can_socket = Comm_Can_SetupSocket(m_ifname.c_str(), m_can_variant);
    if (m_can_socket < 0) {
      std::cout << "Failure to create can socket " << std::endl;
      return;
    }

    m_is_initialized = true;
  }
}

void CanReader::receiveData(frame_t *can_frames, uint8_t num_acf_msgs)
{
  int res = -1;
  // Read acf_num_msgs number of CAN frames from the CAN socket
  int i = 0;
  while (i < num_acf_msgs) {
    // Get payload -- will 'spin' here until we get the requested number
    //                of CAN frames.
    if(m_can_variant == CAN_VARIANT_FD){
        res = read(m_can_socket, &(can_frames[i].fd), sizeof(struct canfd_frame));
    } else {
        res = read(m_can_socket, &(can_frames[i].cc), sizeof(struct can_frame));
    }
    if (!res) {
        std::cout << "Error reading CAN frames" << std::endl;
        continue;
    }
    i++;
  }
}

int CanReader::getCanSocket()
{
  return m_can_socket;
}

void CanReader::stop()
{
  if(m_is_initialized) {
    close(m_can_socket);

    m_is_initialized = false;
  }
}
