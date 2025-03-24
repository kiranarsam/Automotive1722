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

#include "can_codec.h"
#include "can_encode_decode_inl.h"

extern inline float toPhysicalValue(uint64_t target, float factor, float offset, bool is_signed);

extern inline uint64_t fromPhysicalValue(float physical_value, float factor, float offset);

extern inline void clearBits(uint8_t* target_byte, uint8_t* bits_to_clear, const uint8_t startbit, const uint8_t length);

extern inline void storeSignal(uint8_t* frame, uint64_t value, const uint8_t startbit, const uint8_t length,
                        bool is_big_endian, bool is_signed);

extern inline uint64_t extractSignal(const uint8_t* frame, const uint8_t startbit, const uint8_t length, bool is_big_endian,
                              bool is_signed);

extern inline float decode(const uint8_t* frame, const uint16_t startbit, const uint16_t length, bool is_big_endian,
                    bool is_signed, float factor, float offset);

extern inline void encode(uint8_t* frame, const float value, const uint16_t startbit, const uint16_t length,
                   bool is_big_endian, bool is_signed, float factor, float offset);

float Can_Codec_Decode(const uint8_t* frame, const uint16_t startbit, const uint16_t length, bool is_big_endian, bool is_signed, float factor, float offset)
{
  return decode(frame, startbit, length, is_big_endian, is_signed, factor, offset);
}

void Can_Codec_Encode(uint8_t* frame, const float value, const uint16_t startbit, const uint16_t length, bool is_big_endian, bool is_signed, float factor, float offset)
{
  encode(frame, value, startbit, length, is_big_endian, is_signed, factor, offset);
}

float Can_Codec_ToPhysicalValue(uint64_t target, float factor, float offset, bool is_signed)
{
  return toPhysicalValue(target, factor, offset, is_signed);
}

uint64_t Can_Codec_ExtractSignal(const uint8_t* frame, const uint8_t startbit, const uint8_t length, bool is_big_endian, bool is_signed)
{
  return extractSignal(frame, startbit, length, is_big_endian, is_signed);
}
