#pragma once

/* System files */
#include <poll.h>
#include <sys/socket.h>
#include <linux/if_packet.h>

#include <string>
#include <thread>

#include "IeeeCanCommon.hpp"
#include "CanReader.hpp"

class Transmitter
{
public:

  Transmitter(std::string &ifname, std::string &macaddr);

  ~Transmitter();

  void init();

  void start();

  void stop();

  void publish(frame_t *can_frames, uint8_t num_acf_msgs);

private:

  void run();

  std::string m_ifname;

  std::string m_macaddr;

  std::string m_can_ifname;

  int m_eth_fd;

  bool m_is_initialized;

  bool m_is_can_enabled;

  std::thread m_transmitter_thread;

  bool m_running;

  struct pollfd m_poll_fds;

  struct sockaddr_ll sk_ll_addr;

  struct sockaddr* m_dest_addr;

  CanReader m_can_reader;

};