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

#include "CanDbc.hpp"

#include <regex>

CanDbc::CanDbc(const std::string &filename, std::unordered_map<uint32_t, DbCanMessage> &can_db) : m_is_initialized{false}
{
  init(filename, can_db);
}

CanDbc::~CanDbc()
{
}

void CanDbc::init(const std::string &filename, std::unordered_map<uint32_t, DbCanMessage> &can_db)
{
  if (!m_is_initialized) {
    std::ifstream fp(filename);
    if (!fp.is_open()) {
      std::cout << "Error opening file " << filename << std::endl;
      return;
    }

    parse(fp, can_db);

    fp.close();
    m_is_initialized = true;
  }
}

void CanDbc::parseLine(const std::string &line, std::vector<std::string> &tokens)
{
  std::regex pattern("\\S+");
  auto words_begin = std::sregex_iterator(line.begin(), line.end(), pattern);
  auto words_end = std::sregex_iterator();
  for (auto it = words_begin; it != words_end; it++) {
    std::smatch match = *it;
    tokens.push_back(match.str());
  }
}

void CanDbc::parse(std::ifstream &fp, std::unordered_map<uint32_t, DbCanMessage> &can_db)
{
  std::string line {};
  uint32_t frame_id = 0;

  while (std::getline(fp, line)) {
    auto is_bo_section = line.find("BO_");
    auto is_sg_section = line.find("SG_");
    std::vector<std::string> tokens;

    if(is_bo_section != std::string::npos) { // frame section
      parseLine(line, tokens);
      if(tokens.size() >= 5) {
        frame_id = std::stoul(tokens[1]);
        auto frame_name = tokens[2];
        can_db[frame_id] = {frame_id, frame_name.substr(0, frame_name.find(':'))};
      }
    } else if(is_sg_section != std::string::npos) { // signal section
      parseLine(line, tokens);
      auto &message = can_db[frame_id];
      uint8_t index = 1;
      auto signal_name = tokens[index++];
      uint8_t mux_type = 0;
      uint8_t mux_id = 0;
      // Multiplexer
      if(tokens.size() > 8) {
        auto mux = tokens[index++];
        char chMux;
        std::stringstream mux_ss(mux);
        mux_ss >> chMux >> mux_id;
        if('M' == chMux) {
          mux_type = 1;
          message.is_multiplexed = 1;
        } else if('m' == mux[0]) {
          mux_type = 2;
          message.is_multiplexed = 1;
        }
      }
      index++; // ignore ":"
      std::stringstream bits_ss(tokens[index++]);
      int start_bit, signal_len, byte_order;
      char del1, del2, del3;
      bits_ss >> start_bit >> del1 >> signal_len >> del2 >> byte_order >> del3;
      int is_signed = (del3 == '-');

      std::stringstream factor_ss(tokens[index++]);
      float factor, offset;
      factor_ss >> del1 >> factor >> del2 >> offset >> del3;

      std::stringstream minmax_ss(tokens[index++]);
      float min, max;
      minmax_ss >> del1 >> min >> del2 >> max >> del3;

      auto unit = tokens[index++];
      std::string receiver_list = "";
      for (int id = index; id < tokens.size(); id++) {
        receiver_list += tokens[id];
      }

      if (byte_order == 0) {
        // following code is from https://github.com/julietkilo/CANBabel/blob/master/src/main/java/com/github/canbabel/canio/dbc/DbcReader.java:
        int pos = 7 - (start_bit % 8) + (signal_len - 1);
        if (pos < 8) {
            start_bit = start_bit - signal_len + 1;
        } else {
            int cpos = 7 - (pos % 8);
            int bytes = (int)(pos / 8);
            start_bit = cpos + (bytes * 8) + (int)(start_bit/8) * 8;
        }
      }
      message.signals.push_back({signal_name, start_bit, signal_len, (byte_order == 0), is_signed, factor, offset, min, max, unit, receiver_list, mux_type, mux_id});
    }
  }
}