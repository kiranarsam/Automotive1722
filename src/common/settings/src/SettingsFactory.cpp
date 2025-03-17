#include "SettingsFactory.hpp"
#include "Settings.hpp"
#include <memory>

std::shared_ptr<ISettings> SettingsFactory::GetInstance()
{
  static std::shared_ptr<ISettings> settings_instance = std::make_shared<Settings>();
  return settings_instance;
}