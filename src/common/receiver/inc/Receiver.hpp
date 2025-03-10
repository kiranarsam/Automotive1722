#pragma once

/* System files */
#include <poll.h>

#include <string>
#include <thread>

#include "CanWriter.hpp"

class Receiver
{
public:

  Receiver(std::string &ifname, std::string &macaddr);

  ~Receiver();

  void init();

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

  CanWriter m_can_writer;
};