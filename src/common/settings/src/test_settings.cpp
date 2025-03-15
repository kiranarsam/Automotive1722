#include "SettingsFactory.hpp"
#include <iostream>
#include <string>

int main() {

  auto settings = SettingsFactory::GetInstance();

  std::string macaddr;
  std::string channel_name = SettingsKey::CHANNEL + "2";

  settings->getData(channel_name, SettingsKey::DEST_MAC_ADDR, macaddr);

  std::cout << "Mac address = " << macaddr << std::endl;

  std::string num_channel;
  settings->getData(SettingsKey::DEFAULT, SettingsKey::NUM_CHANNEL, num_channel);

  std::cout << "NUM_CHANNEL = " << num_channel << std::endl;

  return 0;
}