
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
