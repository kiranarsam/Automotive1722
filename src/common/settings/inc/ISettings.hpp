#pragma once

#include <string>

enum class SettingsType {
  SETTINGS_TYPE_DEFAULT,
  SETTINGS_TYPE_CHANNEL
};

class SettingsKey final
{
private:
  SettingsKey() = delete;
  ~SettingsKey() = delete;

public:
  static const std::string SECTION_DEFAULT;
  static const std::string NUM_CHANNEL;

  static const std::string SECTION_CHANNEL;
  static const std::string ETH_IF_NAME;
  static const std::string DEST_MAC_ADDR;
  static const std::string CAN_IF_NAME_RECEIVER;
  static const std::string CAN_IF_NAME_TRANSMITTER;
  static const std::string CAN_RECEIVER_ENABLED;
  static const std::string CAN_TRANSMITTER_ENABLED;
};

class ISettings
{
public:
  virtual const std::string getData(const std::string &section, const std::string &key) = 0;
};