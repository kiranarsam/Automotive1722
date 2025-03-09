#pragma once

extern "C" {
#include <stdio.h>
#include "avtp/acf/Can.h"
}

#include "IeeeCanCommon.hpp"

#define STREAM_ID                       0xAABBCCDDEEFF0001
#define USER_CAN_VARIANT                AVTP_CAN_FD

class IeeeUtil final
{
private:

  IeeeUtil() = delete;

  ~IeeeUtil() = delete;

public:

  static int extractCanFramesFromAvtp(uint8_t* pdu, frame_t* can_frames, uint8_t* exp_cf_seqnum);

  static int insertCanFramesToAvtp(uint8_t* pdu, frame_t *can_frames, uint8_t num_acf_msgs, uint8_t cf_seq_num);

private:
  static int isValidAcfPacket(uint8_t *acf_pdu);

  static int initCfPdu(uint8_t* pdu, int use_tscf, int seq_num);

  static int updateCfLength(uint8_t* cf_pdu, uint64_t length, int use_tscf);

  static int prepareAcfPacket(uint8_t* acf_pdu, frame_t* frame, Avtp_CanVariant_t can_variant);

  static Avtp_CanVariant_t getCanVariant(int can_variant);
};