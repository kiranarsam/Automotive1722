
#include "VssMapper.hpp"
#include <iostream>
#include <fstream>

#include "yaml-cpp/yaml.h"

VssMapper::VssMapper() : m_is_initialized{}
{
  init();
}

VssMapper::~VssMapper()
{

}

std::string VssMapper::getDbcNameFromVssMap(const std::string &dbc_name)
{
  return "";
}

std::string VssMapper::getVssNameFromDbcMap(const std::string &vss_name)
{
  return "";
}

const VssMessage VssMapper::getVssMessageForVssName(const std::string &vss_name)
{
  return VssMessage{};
}

void VssMapper::init()
{
  if(!m_is_initialized) {
    m_is_initialized = load();
  }
}

bool VssMapper::load()
{
  std::string filename{"vss.yaml"};
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cout << "Error opening \"" << filename << "\" file" << std::endl;
    return false;
  } else {
    file.close();
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
      }
    }
  }
  node.reset();

  return true;
}
