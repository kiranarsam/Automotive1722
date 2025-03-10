
#ifndef CAN_CODEC_COMMON_H__
#define CAN_CODEC_COMMON_H__

#include <stdint.h>
#include <stdbool.h>

float Can_Codec_Decode(const uint8_t* frame, const uint16_t startbit, const uint16_t length, bool is_big_endian, bool is_signed, float factor, float offset);

void Can_Codec_Encode(uint8_t* frame, const float value, const uint16_t startbit, const uint16_t length, bool is_big_endian, bool is_signed, float factor, float offset);

#endif /* CAN_CODEC_COMMON_H__ */