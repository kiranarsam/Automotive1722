
#pragma once

#include <string>

#include "IeeeCanCommon.hpp"

class CanReader
{
public:
  CanReader();
  ~CanReader();

  void init(std::string &ifname, uint8_t can_variant);

  void receiveData(frame_t *can_frames, uint8_t num_acf_msgs);

  int getCanSocket();

  void stop();

private:

  int m_can_socket;

  std::string m_ifname;

  uint8_t m_can_variant;

  bool m_is_initialized;
};