#include "SettingsFactory.hpp"
#include "Settings.hpp"
#include <memory>

std::shared_ptr<ISettings> SettingsFactory::GetInstance()
{
  static std::shared_ptr<ISettings> s_instance = std::make_shared<Settings>();
  return s_instance;
}