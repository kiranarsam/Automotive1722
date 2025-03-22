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

int main()
{
  std::unordered_map<uint32_t, CanMessage> database;
  std::string filename = "test.dbc";

  CanDbc canDbc{filename, database};

  for (auto &pair : database) {
    std::cout << "Can Message: " << pair.first << std::endl;

    auto &msg = pair.second;
    std::cout << "can_id: " << msg.canID << ", "
              << "name: " << msg.name << ", "
              << "is_multiplexed: " << msg.isMultiplexed << std::endl;

    std::cout << "Signals: " << std::endl;

    for (auto &signal : msg.signals) {
      std::cout << "name: " << signal.name << ", " << signal.startBit << "@" << signal.signalLength << ", "
                << "big_endian: " << signal.is_big_endian << ", "
                << "signed: " << signal.is_signed << ", "
                << "(" << signal.factor << ", " << signal.offset << "), "
                << "[" << signal.min << "|" << signal.max << "], "
                << "unit: " << signal.unit << ", "
                << "receiver_list: " << signal.receiverList << ", "
                << "mux_id: " << signal.muxId << ", "
                << "number: " << signal.number << std::endl;
    }
  }

  return 0;
}