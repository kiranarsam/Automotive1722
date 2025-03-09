

/* System files */
#include <linux/if_ether.h>
#include <unistd.h>

extern "C" {
#include "ether_common.h"
#include "can_common.h"
}

#include <iostream>
#include <functional>

#include "Ieee1722Receiver.hpp"
#include "IeeeUtil.hpp"


#define MAX_ETH_PDU_SIZE                1500
#define MAX_CAN_FRAMES_IN_ACF           15

Ieee1722Receiver::Ieee1722Receiver(std::string &ifname, std::string &macaddr)
  : m_ifname{ifname}, m_macaddr{macaddr}
{
  m_is_can_enabled = false;
  init();
}

Ieee1722Receiver::~Ieee1722Receiver()
{

}

void Ieee1722Receiver::init()
{
  if (!m_is_initialized) {
    int res = -1;
    uint8_t addr[ETH_ALEN];

    res = IeeeUtil::getMacAddress(m_macaddr, addr);
    if(res < 0) {
      std::cout << "Invalid mac address" << std::endl;
      return;
    }

    m_eth_fd = Comm_Ether_CreateListenerSocket(m_ifname.c_str(), addr, ETH_P_TSN);
    if (m_eth_fd < 0) {
      std::cout << "Failue to create listener ethernet socket" << std::endl;
      return;
    }

    // Open a CAN socket for reading frames
    if(m_is_can_enabled) {
      m_can_socket = Comm_Can_SetupSocket(m_can_ifname.c_str(), CAN_VARIANT_FD);
      if (m_can_socket < 0) {
        std::cout << "Failure to create can socket " << std::endl;
        return;
      }
    }

    m_is_initialized = true;
    std::cout << "init initialized " << std::endl;
  }
}

void Ieee1722Receiver::start()
{
  if(m_is_initialized) {
    m_running = true;
    m_receiver_thread = std::thread{std::bind(&Ieee1722Receiver::run, this)};
  }
}

void Ieee1722Receiver::stop()
{
  m_running = false;
  if (std::this_thread::get_id() != m_receiver_thread.get_id()) {
    if (m_receiver_thread.joinable())
    {
      m_receiver_thread.join();
    }
  } else {
    m_receiver_thread.detach();
  }

  if(m_is_can_enabled) {
    close(m_can_socket);
  }
}

void Ieee1722Receiver::run()
{
  int res = 0;
  uint16_t pdu_length = 0;
  uint8_t num_can_msgs = 0;
  uint8_t exp_cf_seqnum = 0;
  uint8_t pdu[MAX_ETH_PDU_SIZE];
  frame_t can_frames[MAX_CAN_FRAMES_IN_ACF];

  m_poll_fds.fd = m_eth_fd;
  m_poll_fds.events = POLLIN;

  while(m_running)
  {
    // add polling to avoid blocking calls
    res = poll(&m_poll_fds, 1, 500);
    if (res < 0) {
      std::cout <<"Failed poll() call" << std::endl;
      // destroy handler
      return;
    }

    // check for m_running
    if(!m_running) {
      // destroy handler
      std::cout <<"Exited thread " << std::endl;
      return;
    }

    if (res == 0) {
      // timeout expired
      continue;
    }

    if (m_poll_fds.revents & POLLIN) {

      pdu_length = recv(m_eth_fd, pdu, MAX_ETH_PDU_SIZE, 0);
      if (pdu_length < 0 || pdu_length > MAX_ETH_PDU_SIZE) {
          std::cout << "Failed to receive data" << std::endl;
          continue;
      }

      num_can_msgs = IeeeUtil::extractCanFramesFromAvtp(pdu, can_frames, &exp_cf_seqnum);

      exp_cf_seqnum++;

      if(!m_is_can_enabled) {
        continue;
      }

      for (int i = 0; i < num_can_msgs; i++) {
          // TODO
          Avtp_CanVariant_t can_variant = AVTP_CAN_FD;

          if (can_variant == AVTP_CAN_FD) {
              res = write(m_can_socket, &can_frames[i].fd, sizeof(struct canfd_frame));
          } else if (can_variant == AVTP_CAN_CLASSIC) {
              res = write(m_can_socket, &can_frames[i].cc, sizeof(struct can_frame));
          }

          if(res < 0) {
              std::cout << "Failed to write to CAN bus" << std::endl;
              continue;
          }
      }
    }
  }
}