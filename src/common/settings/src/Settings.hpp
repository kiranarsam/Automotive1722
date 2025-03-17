#pragma once

#include <fstream>
#include <unordered_map>

#include "ISettings.hpp"

class Settings : public ISettings
{
public:
  Settings();

  ~Settings();

  const std::string getData(const std::string &section, const std::string &key);

private:
  void load();

  void parse(std::ifstream &ins);

  std::string leftTrim(const std::string &token);

  std::string rightTrim(const std::string &token);

  std::string trim(const std::string &token);

  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_settings_map;

  static const std::string WHITESPACE;
  static const std::string SETTINGS_FILE_NAME;
};