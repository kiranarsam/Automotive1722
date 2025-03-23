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

#include <string>
#include <vector>

/* Below structure is used for database, to track with input dbc file */
struct DbCanSignal {
  std::string name;
  int start_bit;
  int signal_length;
  int is_big_endian;
  int is_signed;
  float factor;
  float offset;
  float min;
  float max;
  std::string unit;
  std::string receiver_list;
  uint8_t is_multiplexer;
  uint8_t mux_id;
  uint8_t number;
};

/* Below structure is used for database, to track with input dbc file */
struct DbCanMessage {
  uint32_t can_id;
  std::string name;
  uint8_t is_multiplexed;
  std::vector<DbCanSignal> signals;
};

/* Used to exchange information from manager to agents */
struct CanSignal {
  std::string name;
  float value;
};

/* Used to exchange information from manager to agents */
struct CanMessage {
  uint32_t can_id;
  int type; // CC or FD
  uint8_t len;
  uint32_t cycle_time;
  std::vector<CanSignal> signals;
};
