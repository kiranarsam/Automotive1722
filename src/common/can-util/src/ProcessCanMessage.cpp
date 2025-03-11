
extern "C" {
#include "can_codec.h"
}

#include "ProcessCanMessage.hpp"

void ProcessCanMessage::setValue(std::string &name, uint64_t value, double scaled, can_message &data_out)
{
  for (auto signal = data_out.signals.begin(); signal != data_out.signals.end(); signal++)
  {
    if(name.compare(signal->name) == 0) {
      // update the value
      break;
    }
  }
}

void ProcessCanMessage::process(uint8_t *can_data, can_message &msg, can_message &data_out)
{
	uint64_t value = 0;
	double scaled = 0.;
	unsigned int muxerVal = 0;

  if(msg.isMultiplexed) {
    // find multiplexer:
    for (auto signal = msg.signals.begin(); signal != msg.signals.end(); signal++)
    {
      if (1 == signal->isMultiplexer)
      {
        muxerVal = Can_Codec_ExtractSignal(can_data, signal->startBit, signal->signalLength, (bool) signal->is_big_endian, signal->is_signed);
        scaled = Can_Codec_ToPhysicalValue(muxerVal, signal->factor, signal->offset, signal->is_signed);
        ProcessCanMessage::setValue(signal->name, muxerVal, scaled, data_out);
        break;
      }
    }

    for (auto signal = msg.signals.begin(); signal != msg.signals.end(); signal++)
    {
      // decode not multiplexed signals and signals with correct muxVal
      if (0 == signal->isMultiplexer || (2 == signal->isMultiplexer && signal->muxId == muxerVal))
      {
        value = Can_Codec_ExtractSignal(can_data, signal->startBit, signal->signalLength, (bool) signal->is_big_endian, signal->is_signed);
        scaled = Can_Codec_ToPhysicalValue(value, signal->factor, signal->offset, signal->is_signed);
        ProcessCanMessage::setValue(signal->name, value, scaled, data_out);
      }
    }
  }
  else
  {
    for (auto signal = msg.signals.begin(); signal != msg.signals.end(); signal++)
    {
      value = Can_Codec_ExtractSignal(can_data, signal->startBit, signal->signalLength, (bool) signal->is_big_endian, signal->is_signed);
      scaled = Can_Codec_ToPhysicalValue(value, signal->factor, signal->offset, signal->is_signed);
      ProcessCanMessage::setValue(signal->name, value, scaled, data_out);
    }
  }
}