
#include "VssMapper.hpp"
#include <iostream>

int main()
{
  VssMapper vss_mapper;

  VssMessage vss_msg;
  vss_mapper.getVssMessageFromVssName("Vehicle.Speed", vss_msg);

  std::cout << "vss_name: " << vss_msg.vss_name << std::endl;
  std::cout << "datatype: " << vss_msg.datatype << std::endl;
  std::cout << "type: " << vss_msg.type << std::endl;
  std::cout << "unit: " << vss_msg.unit << std::endl;

  return 0;
}