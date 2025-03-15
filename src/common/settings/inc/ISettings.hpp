#pragma once

#include <string>

enum class SettingsType
{
  SETTINGS_TYPE_DEFAULT,
  SETTINGS_TYPE_CHANNEL
};

class SettingsKey
{
public:
  static const std::string DEFAULT;
  static const std::string NUM_CHANNEL;

  static const std::string CHANNEL;
  static const std::string ETH_IF_NAME;
  static const std::string DEST_MAC_ADDR;
  static const std::string CAN_IF_NAME;
  static const std::string CAN_ENABLED;
};

class ISettings
{
public:
  virtual void getData(const std::string &section, const std::string &key, std::string &value) = 0;
};