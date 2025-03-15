#pragma once

#include <memory>

#include "ISettings.hpp"

class SettingsFactory final
{
private:
  SettingsFactory() = delete;
  ~SettingsFactory() = delete;
public:
  static std::shared_ptr<ISettings> GetInstance();
};
