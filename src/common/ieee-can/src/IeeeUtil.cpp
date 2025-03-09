
/* System defines */
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

extern "C"
{
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "avtp/CommonHeader.h"
#include "avtp/acf/Ntscf.h"
#include "avtp/acf/Tscf.h"
}

#include <iostream>
#include <vector>
#include <regex>

#include "IeeeUtil.hpp"

int IeeeUtil::extractCanFramesFromAvtp(uint8_t* pdu, frame_t* can_frames, uint8_t* exp_cf_seqnum)
{
  uint8_t *cf_pdu = nullptr;
  uint8_t *acf_pdu = nullptr;
  uint8_t *udp_pdu = nullptr;
  uint8_t seq_num = 0;
  uint8_t i = 0;
  uint16_t proc_bytes = 0;
  uint16_t msg_length = 0;
  uint64_t stream_id;

  cf_pdu = pdu;

  // Accept only TSCF and NTSCF formats allowed
  uint8_t subtype = Avtp_CommonHeader_GetSubtype((Avtp_CommonHeader_t*)cf_pdu);
  if (subtype == AVTP_SUBTYPE_TSCF) {
      proc_bytes += AVTP_TSCF_HEADER_LEN;
      msg_length += Avtp_Tscf_GetStreamDataLength((Avtp_Tscf_t*)cf_pdu) + AVTP_TSCF_HEADER_LEN;
      stream_id = Avtp_Tscf_GetStreamId((Avtp_Tscf_t*)cf_pdu);
      seq_num = Avtp_Tscf_GetSequenceNum((Avtp_Tscf_t*)cf_pdu);
  } else if (subtype == AVTP_SUBTYPE_NTSCF) {
      proc_bytes += AVTP_NTSCF_HEADER_LEN;
      msg_length += Avtp_Ntscf_GetNtscfDataLength((Avtp_Ntscf_t*)cf_pdu) + AVTP_NTSCF_HEADER_LEN;
      stream_id = Avtp_Ntscf_GetStreamId((Avtp_Ntscf_t*)cf_pdu);
      seq_num = Avtp_Ntscf_GetSequenceNum((Avtp_Ntscf_t*)cf_pdu);
  } else {
      return -1;
  }

  // Check for stream id
  if (stream_id != STREAM_ID) {
      return -1;
  }

  // Check sequence numbers.
  if (seq_num != *exp_cf_seqnum) {
      std::cout << "Incorrect sequence num. Expected:"
                << *exp_cf_seqnum << " Recd.: " << seq_num << std::endl;

      *exp_cf_seqnum = seq_num;
  }

  while (proc_bytes < msg_length)
  {
      acf_pdu = &pdu[proc_bytes];

      if (!IeeeUtil::isValidAcfPacket(acf_pdu)) {
          return -1;
      }

      canid_t can_id = Avtp_Can_GetCanIdentifier((Avtp_Can_t*)acf_pdu);
      uint8_t* can_payload = Avtp_Can_GetPayload((Avtp_Can_t*)acf_pdu);
      uint16_t acf_msg_length = Avtp_Can_GetAcfMsgLength((Avtp_Can_t*)acf_pdu)*4;
      uint16_t can_payload_length = Avtp_Can_GetCanPayloadLength((Avtp_Can_t*)acf_pdu);
      proc_bytes += acf_msg_length;

      // Need to work on multiple can frames
      frame_t* frame = &(can_frames[i++]);

      // Handle EFF Flag
      if (Avtp_Can_GetEff((Avtp_Can_t*)acf_pdu)) {
          can_id |= CAN_EFF_FLAG;
      } else if (can_id > 0x7FF) {
          std::cout << "Error: CAN ID is > 0x7FF but the EFF bit is not set." << std::endl;
          return -1;
      }

      // Handle RTR Flag
      if (Avtp_Can_GetRtr((Avtp_Can_t*)acf_pdu)) {
          can_id |= CAN_RTR_FLAG;
      }

      if (USER_CAN_VARIANT == AVTP_CAN_FD) {
          if (Avtp_Can_GetBrs((Avtp_Can_t*)acf_pdu)) {
              frame->fd.flags |= CANFD_BRS;
          }
          if (Avtp_Can_GetFdf((Avtp_Can_t*)acf_pdu)) {
              frame->fd.flags |= CANFD_FDF;
          }
          if (Avtp_Can_GetEsi((Avtp_Can_t*)acf_pdu)) {
              frame->fd.flags |= CANFD_ESI;
          }

          frame->fd.can_id = can_id;
          frame->fd.len = can_payload_length;
          memcpy(frame->fd.data, can_payload, can_payload_length);
      } else {
          frame->cc.can_id = can_id;
          frame->cc.len = can_payload_length;
          memcpy(frame->cc.data, can_payload, can_payload_length);
      }

      std::cout << "canid = " << can_id << ", length = " << can_payload_length << std::endl;
  }

  return i;
}

int IeeeUtil::insertCanFramesToAvtp(uint8_t* pdu, frame_t *can_frames,
    uint8_t num_acf_msgs, uint8_t cf_seq_num)
{
  // Pack into control formats
  uint8_t *cf_pdu = nullptr;
  uint16_t pdu_length = 0;
  uint16_t cf_length = 0;
  int res = -1;

  // TODO
  int use_tscf = 0;

  // Prepare the control format: TSCF/NTSCF
  cf_pdu = pdu + pdu_length;
  res = IeeeUtil::initCfPdu(cf_pdu, use_tscf, cf_seq_num++);
  pdu_length += res;
  cf_length += res;

  int i = 0;
  while (i < num_acf_msgs) {
      uint8_t* acf_pdu = pdu + pdu_length;
      res = IeeeUtil::prepareAcfPacket(acf_pdu, &(can_frames[i]), USER_CAN_VARIANT);
      pdu_length += res;
      cf_length += res;
      i++;
  }

  // Update the length of the PDU
  IeeeUtil::updateCfLength(cf_pdu, cf_length, use_tscf);

  return pdu_length;
}

int IeeeUtil::isValidAcfPacket(uint8_t *acf_pdu)
{
    Avtp_AcfCommon_t *pdu = (Avtp_AcfCommon_t*) acf_pdu;
    uint8_t acf_msg_type = Avtp_AcfCommon_GetAcfMsgType(pdu);
    if (acf_msg_type != AVTP_ACF_TYPE_CAN) {
        return 0;
    }

    return 1;
}

int IeeeUtil::initCfPdu(uint8_t* pdu, int use_tscf, int seq_num)
{
    int res;
    if (use_tscf) {
        Avtp_Tscf_t* tscf_pdu = (Avtp_Tscf_t*) pdu;
        memset(tscf_pdu, 0, AVTP_TSCF_HEADER_LEN);
        Avtp_Tscf_Init(tscf_pdu);
        Avtp_Tscf_SetField(tscf_pdu, AVTP_TSCF_FIELD_TU, 0U);
        Avtp_Tscf_SetField(tscf_pdu, AVTP_TSCF_FIELD_SEQUENCE_NUM, seq_num);
        Avtp_Tscf_SetField(tscf_pdu, AVTP_TSCF_FIELD_STREAM_ID, STREAM_ID);
        res = AVTP_TSCF_HEADER_LEN;
    } else {
        Avtp_Ntscf_t* ntscf_pdu = (Avtp_Ntscf_t*) pdu;
        memset(ntscf_pdu, 0, AVTP_NTSCF_HEADER_LEN);
        Avtp_Ntscf_Init(ntscf_pdu);
        Avtp_Ntscf_SetField(ntscf_pdu, AVTP_NTSCF_FIELD_SEQUENCE_NUM, seq_num);
        Avtp_Ntscf_SetField(ntscf_pdu, AVTP_NTSCF_FIELD_STREAM_ID, STREAM_ID);
        res = AVTP_NTSCF_HEADER_LEN;
    }
    return res;
}

int IeeeUtil::updateCfLength(uint8_t* cf_pdu, uint64_t length, int use_tscf)
{
    if (use_tscf) {
        uint64_t payloadLen = length - AVTP_TSCF_HEADER_LEN;
        Avtp_Tscf_SetField((Avtp_Tscf_t*)cf_pdu, AVTP_TSCF_FIELD_STREAM_DATA_LENGTH, payloadLen);
    } else {
        uint64_t payloadLen = length - AVTP_NTSCF_HEADER_LEN;
        Avtp_Ntscf_SetField((Avtp_Ntscf_t*)cf_pdu, AVTP_NTSCF_FIELD_NTSCF_DATA_LENGTH, payloadLen);
    }
    return 0;
}

int IeeeUtil::prepareAcfPacket(uint8_t* acf_pdu, frame_t* frame, Avtp_CanVariant_t can_variant)
{
  struct timespec now;
  canid_t can_id = 0U;
  uint8_t can_payload_length = 0U;

  // Clear bits
  Avtp_Can_t* pdu = (Avtp_Can_t*) acf_pdu;
  memset(pdu, 0, AVTP_CAN_HEADER_LEN);

  // Prepare ACF PDU for CAN
  Avtp_Can_Init(pdu);
  clock_gettime(CLOCK_REALTIME, &now);
  Avtp_Can_SetMessageTimestamp(pdu, (uint64_t)now.tv_nsec + (uint64_t)(now.tv_sec * 1e9));
  Avtp_Can_EnableMtv(pdu);

  // Set required CAN Flags
  if (can_variant == AVTP_CAN_FD) {
    can_id = frame->fd.can_id;
    can_payload_length = frame->fd.len;
  } else {
    can_id = frame->cc.can_id;
    can_payload_length = frame->cc.len;
  }

  if (can_id & CAN_EFF_FLAG) {
      Avtp_Can_EnableEff(pdu);
  }

  if (can_id & CAN_RTR_FLAG) {
      Avtp_Can_EnableRtr(pdu);
  }

  if (can_variant == AVTP_CAN_FD) {
      if (frame->fd.flags & CANFD_BRS) {
          Avtp_Can_EnableBrs(pdu);
      }
      if (frame->fd.flags & CANFD_FDF) {
          Avtp_Can_EnableFdf(pdu);
      }
      if (frame->fd.flags & CANFD_ESI) {
          Avtp_Can_EnableEsi(pdu);
      }
  }

  // Copy payload to ACF CAN PDU
  if(can_variant == AVTP_CAN_FD) {
      Avtp_Can_CreateAcfMessage(pdu, can_id & CAN_EFF_MASK, frame->fd.data,
                                        can_payload_length, can_variant);
  } else {
      Avtp_Can_CreateAcfMessage(pdu, can_id & CAN_EFF_MASK, frame->cc.data,
                                        can_payload_length, can_variant);
  }

  return Avtp_Can_GetAcfMsgLength(pdu)*4;
}

Avtp_CanVariant_t IeeeUtil::getCanVariant(int can_variant)
{
  if (can_variant == 0) {
    return AVTP_CAN_CLASSIC;
  }

  return AVTP_CAN_FD;
}

int IeeeUtil::getMacAddress(std::string &macaddr, uint8_t *address)
{
  std::regex mac_addr_regex("^([0-9A-Fa-f]{2})(:([0-9A-Fa-f]{2})){5}$");
  bool is_matched = std::regex_match(macaddr, mac_addr_regex);
  if (is_matched)
  {
    std::regex delimiter(":");
    std::sregex_token_iterator it(macaddr.begin(), macaddr.end(), delimiter, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> tokens;
    for (; it != end; ++it)
    {
      tokens.push_back((*it));
    }

    if (tokens.size() == ETH_ALEN)
    {
      for (int i = 0; i < tokens.size(); i++)
      {
        address[i] = static_cast<uint8_t>(std::stoul(tokens[i], 0, 16));
      }
      return 0;
    }
  }
  return -1;
}