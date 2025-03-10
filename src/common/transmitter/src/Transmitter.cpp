
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

Transmitter::Transmitter(std::string &ifname, std::string &macaddr)
  : m_ifname{ifname}, m_macaddr{macaddr}
{
  m_is_can_enabled = false;
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

    // Open a CAN socket for reading frames
    if(m_is_can_enabled) {
      m_can_reader.init(m_can_ifname, CAN_VARIANT_FD);
    }

    m_is_initialized = true;
    std::cout << "Talker initialized" << std::endl;
  }
}

void Transmitter::start()
{
  if(m_is_initialized && m_is_can_enabled) {
    m_running = true;
    m_transmitter_thread = std::thread{std::bind(&Transmitter::run, this)};
  }
}

void Transmitter::stop()
{
  m_running = false;

  if(m_is_can_enabled) {
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

void Transmitter::publish(frame_t *can_frames, uint8_t num_acf_msgs)
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