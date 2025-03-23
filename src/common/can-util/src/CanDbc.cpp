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

#include <cstdio>

CanDbc::CanDbc(std::string &filename, std::unordered_map<uint32_t, DbCanMessage> &can_db) : m_is_initialized{false}
{
  init(filename, can_db);
}

CanDbc::~CanDbc()
{
}

void CanDbc::init(std::string &filename, std::unordered_map<uint32_t, DbCanMessage> &can_db)
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

/*
 * Copyright (c) 2025, Kiran Kumar Arsam,
 * Copyright (c) 2016, Eduard Bröcker,
 * All rights reserved.
 * Content is simplified to C++ usage.
 */
void CanDbc::parse(std::ifstream &fp, std::unordered_map<uint32_t, DbCanMessage> &can_db)
{
  char frame_name[512], sender[512];
  char signal_name[512], signed_state, unit[512], receiver_list[512];
  int start_bit = 0, signal_length = 0, byte_order = 0;
  float factor = 0., offset = 0., min = 0., max = 0.;
  char mux[4];
  int mux_id = 0;
  uint32_t frame_id = 0, len = 0;

  std::string line{};

  while (std::getline(fp, line)) {
    if (std::sscanf(line.c_str(), " BO_ %d %s %d %s", &frame_id, frame_name, &len, sender) == 4) {
      can_db[frame_id] = {frame_id, std::string{frame_name}};
    } else if (std::sscanf(line.c_str(), " SG_ %s : %d|%d@%d%c (%f,%f) [%f|%f] %s %s", signal_name, &start_bit,
                           &signal_length, &byte_order, &signed_state, &factor, &offset, &min, &max, unit,
                           receiver_list) > 5) {
      if (byte_order == 0) {
        int pos = 7 - (start_bit % 8) + (signal_length - 1);
        if (pos < 8) {
          start_bit = start_bit - signal_length + 1;
        } else {
          int cpos = 7 - (pos % 8);
          int bytes = (int)(pos / 8);
          start_bit = cpos + (bytes * 8) + (int)(start_bit / 8) * 8;
        }
      }

      auto &message = can_db[frame_id];
      message.signals.push_back({std::string{signal_name}, start_bit, signal_length, (byte_order == 0),
                                 (signed_state == '-'), factor, offset, min, max, std::string{unit},
                                 std::string{receiver_list}, 0, 0});
    } else if (std::sscanf(line.c_str(), " SG_ %s %s : %d|%d@%d%c (%f,%f) [%f|%f] %s %s", signal_name, mux, &start_bit,
                           &signal_length, &byte_order, &signed_state, &factor, &offset, &min, &max, unit,
                           receiver_list) > 5) {
      if (byte_order == 0) {
        int pos = 7 - (start_bit % 8) + (signal_length - 1);
        if (pos < 8) {
          start_bit = start_bit - signal_length + 1;
        } else {
          int cpos = 7 - (pos % 8);
          int bytes = (int)(pos / 8);
          start_bit = cpos + (bytes * 8) + (int)(start_bit / 8) * 8;
        }
      }
      auto &message = can_db[frame_id];
      if (mux[0] == 'M') {
        message.signals.push_back({std::string{signal_name}, start_bit, signal_length, (byte_order == 0),
                                   (signed_state == '-'), factor, offset, min, max, std::string{unit},
                                   std::string{receiver_list}, 1, 0});
        message.is_multiplexed = 1;
      } else if (mux[0] == 'm') {
        std::sscanf(mux, "m%d", &mux_id);
        message.signals.push_back({std::string{signal_name}, start_bit, signal_length, (byte_order == 0),
                                   (signed_state == '-'), factor, offset, min, max, std::string{unit},
                                   std::string{receiver_list}, 2, (uint8_t)mux_id});
        message.is_multiplexed = 1;
      }
    }
  }
}