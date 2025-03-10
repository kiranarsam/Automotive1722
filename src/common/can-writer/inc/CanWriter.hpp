
#pragma once

#include <string>

#include "IeeeCanCommon.hpp"

class CanWriter
{
public:
  CanWriter();
  ~CanWriter();

  void init(std::string &ifname, uint8_t can_variant);

  void sendData(frame_t *can_frames, uint8_t num_can_msgs);

  void stop();

private:

  int m_can_socket;

  std::string m_ifname;

  uint8_t m_can_variant;

  bool m_is_initialized;
};