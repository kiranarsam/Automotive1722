
#include "CanWriter.hpp"

#include <unistd.h>

extern "C" {
  #include "can_comm_if.h"
}

#include <iostream>

CanWriter::CanWriter()
  : m_ifname{}, m_can_variant{}, m_can_socket{-1}, m_is_initialized{false}
{

}

CanWriter::~CanWriter()
{

}

void CanWriter::init(std::string &ifname, uint8_t can_variant)
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

void CanWriter::sendData(frame_t *can_frames, uint8_t num_can_msgs)
{
  int res = -1;
  for (int i = 0; i < num_can_msgs; i++) {
    if (m_can_variant == CAN_VARIANT_FD) {
        res = write(m_can_socket, &can_frames[i].fd, sizeof(struct canfd_frame));
    } else if (m_can_variant == CAN_VARIANT_CC) {
        res = write(m_can_socket, &can_frames[i].cc, sizeof(struct can_frame));
    }

    if(res < 0) {
        std::cout << "Failed to write to CAN bus" << std::endl;
        continue;
    }
  }
}

void CanWriter::stop()
{
  if(m_is_initialized) {
    close(m_can_socket);

    m_is_initialized = false;
  }
}
