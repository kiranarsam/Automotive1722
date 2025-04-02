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

#include "VssMapper.hpp"
#include <iostream>
#include <fstream>

#include "yaml-cpp/yaml.h"

VssMapper::VssMapper(bool generate_json) : m_is_initialized{}, m_gen_vss_json{generate_json}
{
  init();
}

VssMapper::~VssMapper()
{

}

void VssMapper::stop()
{
  m_vss2dbc_map.clear();
  m_dbc2vss_map.clear();
  m_db_cache_map.clear();
}

void VssMapper::getDbcNameFromVssMap(const std::string &vss_name, std::string &dbc_name)
{
  try {
    dbc_name = m_vss2dbc_map.at(vss_name);
  }
  catch(...) {
    dbc_name = "";
  }
}

void VssMapper::getVssNameFromDbcMap(const std::string &dbc_name, std::string &vss_name)
{
  try {
    vss_name = m_dbc2vss_map.at(dbc_name);
  }
  catch(...) {
    vss_name = "";
  }
}

void VssMapper::getVssMessageFromVssName(const std::string &vss_name, VssMessage& vss_msg)
{
  try {
    vss_msg = m_db_cache_map.at(vss_name);
  }
  catch(...) {
    std::cout << "vss message structure doesn't exists in the cache " << std::endl;
  }
}

void VssMapper::init()
{
  if(!m_is_initialized) {
    m_is_initialized = loadVssPathYaml("vss.yaml");
    if(!m_gen_vss_json) {
      loadVssDbcMapper("vss_dbc_mapper.json");
    }
  }
}

bool VssMapper::isFileExists(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cout << "Error opening \"" << filename << "\" file" << std::endl;
    return false;
  }

  file.close();
  return true;
}

bool VssMapper::loadVssPathYaml(const std::string& filename)
{
  if(!isFileExists(filename)) {
    return false;
  }

  std::ofstream file_out;
  if(m_gen_vss_json) {
    file_out.open("vss_out.json", std::ios_base::out);
    if (!file_out.is_open()) {
      std::cout << "Error opening \"vss_out.json\" file" << std::endl;
      return false;
    }
    file_out << "{\n  \"Version\" : \"1.0\"\n";
  }

  YAML::Node node = YAML::LoadFile(filename);
  if (node.IsMap()) {
    for(auto it = node.begin(); it != node.end(); ++it) {
      std::string key = it->first.as<std::string>();
      YAML::Node value = it->second;
      if(value.IsMap()) {
        std::string type = value["type"].as<std::string>();
        if(type.compare("branch") == 0) {
          continue;
        }
        std::string datatype = value["datatype"].as<std::string>();
        std::string unit {};
        if(value["unit"].IsDefined()) {
          unit = value["unit"].as<std::string>();
        }
        m_db_cache_map[key] = {key, datatype, type, unit};
        if (m_gen_vss_json) {
          file_out << "  ,\"" << key << "\" : \"\"\n";
        }
      }
    }
  }

  if (m_gen_vss_json) {
    file_out << "}";
    file_out.close();
    std::cout << "Generated vss_out.json file. Rename this file to vss_dbc_mapper.json update accordingly." << std::endl;
  }
  node.reset();

  return true;
}

void VssMapper::loadVssDbcMapper(const std::string& filename)
{
  if(!isFileExists(filename)) {
    return;
  }

  YAML::Node node = YAML::LoadFile(filename);
  if (node.IsMap()) {
    for(auto it = node.begin(); it != node.end(); ++it) {
      std::string vss_name = it->first.as<std::string>();
      std::string dbc_name = it->second.as<std::string>();
      m_vss2dbc_map[vss_name] = dbc_name;
      if(!dbc_name.empty()) {
        m_dbc2vss_map[dbc_name] = vss_name;
      }
    }
  }
  node.reset();
}
