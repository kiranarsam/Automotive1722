#pragma once

#include "can_dbc_common.hpp"
#include "IeeeCanCommon.hpp"

class ProcessCanMessage final
{
private:
  ProcessCanMessage() = delete;
  ~ProcessCanMessage() = delete;
public:
  static void process(uint8_t *can_data, can_message &msg, can_message &data_out);
private:
  static void setValue(std::string &name, uint64_t value, double scaled, can_message &data_out);
};