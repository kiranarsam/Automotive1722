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