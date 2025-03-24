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

extern "C" {
#include "can_codec.h"
}

#include "ProcessCanMessage.hpp"
#include <iostream>

void ProcessCanMessage::setValue(const std::string name, uint64_t value, double scaled, CanMessage &data_out)
{
  data_out.signals.push_back({name, (float)scaled});
}

/*
 * Copyright (c) 2025, Kiran Kumar Arsam,
 * Copyright (c) 2016, Eduard Bröcker,
 * All rights reserved.
 * Content is simplified to C++ usage.
 */
void ProcessCanMessage::process(uint8_t *can_data, DbCanMessage &msg, CanMessage &data_out)
{
  uint64_t value = 0;
  double scaled = 0.;
  unsigned int muxer_val = 0;

  if (msg.is_multiplexed) {
    // find multiplexer:
    for (auto signal = msg.signals.begin(); signal != msg.signals.end(); signal++) {
      if (1 == signal->is_multiplexer) {
        muxer_val = Can_Codec_ExtractSignal(can_data, signal->start_bit, signal->signal_length,
                                           (bool)signal->is_big_endian, signal->is_signed);
        scaled = Can_Codec_ToPhysicalValue(muxer_val, signal->factor, signal->offset, signal->is_signed);
        ProcessCanMessage::setValue(signal->name, muxer_val, scaled, data_out);
        break;
      }
    }

    for (auto signal = msg.signals.begin(); signal != msg.signals.end(); signal++) {
      // decode not multiplexed signals and signals with correct muxVal
      if (0 == signal->is_multiplexer || (2 == signal->is_multiplexer && signal->mux_id == muxer_val)) {
        value = Can_Codec_ExtractSignal(can_data, signal->start_bit, signal->signal_length, (bool)signal->is_big_endian,
                                        signal->is_signed);
        scaled = Can_Codec_ToPhysicalValue(value, signal->factor, signal->offset, signal->is_signed);
        ProcessCanMessage::setValue(signal->name, value, scaled, data_out);
      }
    }
  } else {
    for (auto signal = msg.signals.begin(); signal != msg.signals.end(); signal++) {
      value = Can_Codec_ExtractSignal(can_data, signal->start_bit, signal->signal_length, (bool)signal->is_big_endian,
                                      signal->is_signed);
      scaled = Can_Codec_ToPhysicalValue(value, signal->factor, signal->offset, signal->is_signed);
      ProcessCanMessage::setValue(signal->name, value, scaled, data_out);
    }
  }
}