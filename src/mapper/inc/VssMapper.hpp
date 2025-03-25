
#pragma once

#include <unordered_map>
#include <string>

typedef struct VssMessageTag {
  std::string vss_name;
  std::string datatype;
  std::string type;
  std::string unit;
} VssMessage;

class VssMapper {
public:
  VssMapper();
  ~VssMapper();

  std::string getDbcNameFromVssMap(const std::string &dbc_name);
  std::string getVssNameFromDbcMap(const std::string &vss_name);
  const VssMessage getVssMessageForVssName(const std::string &vss_name);

private:
  void init();
  bool load();

  bool m_is_initialized;

  std::unordered_map<std::string, std::string> m_dbc2vss_map;
  std::unordered_map<std::string, std::string> m_vss2dbc_map;
  std::unordered_map<std::string, VssMessage> m_db_cache_map;
};