/**
 * Copyright (c) 2025, Kiran Kumar Arsam
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *   * this list of conditions and the following disclaimer in the documentation
 *   * and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Kiran Kumar Arsam nor the names of its
 *   * contributors may be used to endorse or promote products derived from
 *   * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "Settings.hpp"

#include <iostream>
#include <regex>
#include <vector>

const std::string SettingsKey::SECTION_DEFAULT = "DEFAULT";
const std::string SettingsKey::NUM_CHANNEL = "NUM_CHANNEL";

const std::string SettingsKey::SECTION_CHANNEL = "CHANNEL";
const std::string SettingsKey::ETH_IF_NAME = "ETH_IF_NAME";
const std::string SettingsKey::DEST_MAC_ADDR = "DEST_MAC_ADDR";
const std::string SettingsKey::CAN_IF_NAME_RECEIVER = "CAN_IF_NAME_RECEIVER";
const std::string SettingsKey::CAN_IF_NAME_TRANSMITTER = "CAN_IF_NAME_TRANSMITTER";
const std::string SettingsKey::CAN_RECEIVER_ENABLED = "CAN_RECEIVER_ENABLED";
const std::string SettingsKey::CAN_TRANSMITTER_ENABLED = "CAN_TRANSMITTER_ENABLED";

const std::string Settings::WHITESPACE = " \n\r\t\f\v";
const std::string Settings::SETTINGS_FILE_NAME = "settings.dat";

Settings::Settings() : m_settings_map{}
{
  load();
}

Settings::~Settings()
{
}

const std::string Settings::getData(const std::string &section, const std::string &key)
{
  try {
    return m_settings_map.at(section).at(key);
  } catch (...) {
    // return default value in case of exception
    return "";
  }
}

void Settings::load()
{
  bool is_file_parsed = false;
  std::vector<std::string> files = {Settings::SETTINGS_FILE_NAME, "./settings.dat", "/etc/settings/settings.dat"};
  for (const auto &file : files) {
    std::ifstream in_file(file);
    if (in_file.is_open()) {
      parse(in_file);
      in_file.close();
      is_file_parsed = true;
    }
  }

  if (!is_file_parsed) {
    std::cout << "Error opening settings.dat file " << std::endl;
  }
}

void Settings::parse(std::ifstream &ins)
{
  std::string line;
  std::string section_name;
  std::regex re("=");
  while (std::getline(ins, line)) {
    bool is_section = false;
    if ((line.find('[') != std::string::npos) && (line.rfind(']') != std::string::npos)) {
      is_section = true;
    }

    if (is_section) {
      int start_pos = line.find('[') + 1;
      int end_pos = line.rfind(']') - 1;
      section_name = line.substr(start_pos, end_pos);
    } else {
      std::sregex_token_iterator it(line.begin(), line.end(), re, -1);
      std::sregex_token_iterator end;

      std::vector<std::string> tokens;
      for (; it != end; ++it) {
        tokens.push_back(trim(*it));
      }

      if (tokens.size() != 2) {
        continue;
      }

      m_settings_map[section_name].insert(std::make_pair(tokens[0], tokens[1]));
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
