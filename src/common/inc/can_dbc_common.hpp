
#pragma once

#include <string>
#include <vector>

struct can_signal {
	std::string name;
	int startBit;
	int signalLength;
	int is_big_endian;
	int is_signed;
	float factor;
	float offset;
	float min;
	float max;
	std::string unit;
	std::string receiverList;
	uint8_t isMultiplexer;
	uint8_t muxId;
	uint8_t number;
};

struct can_message {
	uint32_t canID;
	std::string name;
  uint8_t isMultiplexed;
	std::vector<can_signal> signals;
};