
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
  VssMapper(bool generate_json = false);
  ~VssMapper();

  void stop();

  void getDbcNameFromVssMap(const std::string &vss_name, std::string &dbc_name);
  void getVssNameFromDbcMap(const std::string &dbc_name, std::string &vss_name);
  void getVssMessageFromVssName(const std::string &vss_name, VssMessage& vss_msg);

private:
  void init();
  bool loadVssPathYaml(const std::string &filename);
  void loadVssDbcMapper(const std::string &filename);
  bool isFileExists(const std::string &filename);

  bool m_is_initialized;
  bool m_gen_vss_json;

  std::unordered_map<std::string, std::string> m_dbc2vss_map;
  std::unordered_map<std::string, std::string> m_vss2dbc_map;
  std::unordered_map<std::string, VssMessage> m_db_cache_map;
};