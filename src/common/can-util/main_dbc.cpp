#include "CanDbc.hpp"


int main()
{
  std::map<uint32_t, can_message> database;
  std::string filename = "test.dbc";

  CanDbc canDbc{filename, database};

  for(auto &pair : database) {
    std::cout << "Can Message: " << pair.first << std::endl;

    auto &msg = pair.second;
    std::cout << "can_id: "<< msg.canID << ", "
              << "name: " << msg.name << ", "
              << "is_multiplexed: " << msg.isMultiplexed << std::endl;

    std::cout << "Signals: " << std::endl;

    for(auto & signal : msg.signals) {
      std::cout << "name: " << signal.name << ", "
                << signal.startBit << "@"
                << signal.signalLength << ", "
                << "big_endian: " << signal.is_big_endian << ", "
                << "signed: " << signal.is_signed << ", "
                << "(" << signal.factor << ", "
                << signal.offset << "), "
                << "[" << signal.min << "|"
                << signal.max << "], "
                << "unit: " << signal.unit << ", "
                << "receiver_list: " << signal.receiverList << ", "
                << "mux_id: " << signal.muxId << ", "
                << "number: " << signal.number
                << std::endl;
    }
  }

  return 0;
}