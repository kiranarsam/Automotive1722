#include "can_codec.h"
#include "can_encode_decode_inl.h"

float Can_Codec_Decode(const uint8_t* frame, const uint16_t startbit, const uint16_t length, bool is_big_endian, bool is_signed, float factor, float offset)
{
  return decode(frame, startbit, length, is_big_endian, is_signed, factor, offset);
}

void Can_Codec_Encode(uint8_t* frame, const float value, const uint16_t startbit, const uint16_t length, bool is_big_endian, bool is_signed, float factor, float offset)
{
  encode(frame, value, startbit, length, is_big_endian, is_signed, factor, offset);
}
