
#pragma once

#include <iostream>
#include <map>
#include <utility>
#include <fstream>

#include "can_dbc_common.hpp"

class CanDbc
{
public:
  CanDbc(std::string &filename, std::map<uint32_t, can_message> &can_db);
  ~CanDbc();

private:
  void init(std::string &filename, std::map<uint32_t, can_message> &can_db);

  void parse(std::ifstream &fp, std::map<uint32_t, can_message> &can_db);

  bool m_is_initialized;
};