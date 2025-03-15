#include "Settings.hpp"

#include <regex>
#include <vector>
#include <iostream>

const std::string SettingsKey::DEFAULT = "DEFAULT";
const std::string SettingsKey::NUM_CHANNEL = "NUM_CHANNEL";

const std::string SettingsKey::CHANNEL = "CHANNEL";
const std::string SettingsKey::ETH_IF_NAME = "ETH_IF_NAME";
const std::string SettingsKey::DEST_MAC_ADDR = "DEST_MAC_ADDR";
const std::string SettingsKey::CAN_IF_NAME = "CAN_IF_NAME";
const std::string SettingsKey::CAN_ENABLED = "CAN_ENABLED";

const std::string Settings::WHITESPACE = " \n\r\t\f\v";
const std::string Settings::SETTINGS_FILE_NAME = "settings.dat";

Settings::Settings() : m_settings_map{}
{
  load();
}

Settings::~Settings()
{
}

void Settings::getData(const std::string &section, const std::string &key, std::string &value)
{
  try {
    value = m_settings_map.at(section).at(key);
  }
  catch(...) {
    value = "";
  }
}

void Settings::load()
{
  bool is_file_parsed = false;
  std::vector<std::string> files = {Settings::SETTINGS_FILE_NAME,
                                    "./settings.dat", "/etc/settings/settings.dat"};
  for (const auto &file : files)
  {
    std::ifstream in_file(file);
    if (in_file.is_open())
    {
      parse(in_file);
      in_file.close();
      is_file_parsed = true;
    }
  }

  if (!is_file_parsed)
  {
    std::cout << "Error opening settings.dat file " << std::endl;
  }
}

void Settings::parse(std::ifstream &ins)
{
  std::string line;
  std::string section_name;
  std::regex re("=");
  while (std::getline(ins, line))
  {
    bool is_section = false;
    if ((line.find('[') != std::string::npos) &&
        (line.rfind(']') != std::string::npos))
    {
      is_section = true;
    }

    if (is_section)
    {
      int start_pos = line.find('[') + 1;
      int end_pos = line.rfind(']') - 1;
      section_name = line.substr(start_pos, end_pos);
    }
    else
    {
      std::sregex_token_iterator it(line.begin(), line.end(), re, -1);
      std::sregex_token_iterator end;

      std::vector<std::string> tokens;
      for (; it != end; ++it)
      {
        tokens.push_back(trim(*it));
      }

      if (tokens.size() != 2)
      {
        continue;
      }

      m_settings_map[section_name].insert(std::make_pair(tokens[0], tokens[1]));

      // auto it1 = m_channel_map.find(section_name);
      // if(it1 != m_channel_map.end()) {
      //   auto &inner_map = it1->second;
      //   inner_map.insert(std::make_pair(tokens[0], tokens[1]));
      // }
      // else {
      //   m_channel_map[section_name].insert(std::make_pair(tokens[0], tokens[1]));
      // }
    }
  }
}

std::string Settings::leftTrim(const std::string &token)
{
  size_t start = token.find_first_not_of(Settings::WHITESPACE);
  return (start == std::string::npos) ? "" : token.substr(start);
}

std::string Settings::rightTrim(const std::string &token)
{
  size_t end = token.find_last_not_of(Settings::WHITESPACE);
  return (end == std::string::npos) ? "" : token.substr(0, end + 1);
}

std::string Settings::trim(const std::string &token)
{
  return rightTrim(leftTrim(token));
}
