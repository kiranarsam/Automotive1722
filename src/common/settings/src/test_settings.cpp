#include "SettingsFactory.hpp"
#include <iostream>
#include <string>

int main() {

  auto settings = SettingsFactory::GetInstance();

  // std::string channel_name = SettingsKey::SECTION_CHANNEL + "2";

  auto macaddr = settings->getData(SettingsKey::SECTION_CHANNEL + "2", SettingsKey::DEST_MAC_ADDR);

  std::cout << "Mac address = " << macaddr << std::endl;

  auto num_channel = settings->getData(SettingsKey::SECTION_DEFAULT, SettingsKey::NUM_CHANNEL);

  std::cout << "NUM_CHANNEL = " << num_channel << std::endl;

  return 0;
}