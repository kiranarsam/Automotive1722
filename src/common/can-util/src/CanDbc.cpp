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
#include <cstdio>

CanDbc::CanDbc(std::string &filename, std::map<uint32_t, can_message> &can_db)
  : m_is_initialized {false}
{
  init(filename, can_db);
}

CanDbc::~CanDbc()
{

}

void CanDbc::init(std::string &filename, std::map<uint32_t, can_message> &can_db)
{
  if(!m_is_initialized)
  {
    std::ifstream fp(filename);
    if (!fp.is_open()) {
      std::cout << "Error opening file " << filename << std::endl;
      return;
    }

    parse(fp, can_db);

    fp.close();
    m_is_initialized = true;
  }
}

/*
 * Copyright (c) 2025, Kiran Kumar Arsam,
 * Copyright (c) 2016, Eduard Bröcker,
 * All rights reserved.
 * Content is simplified to C++ usage.
 */
void CanDbc::parse(std::ifstream &fp, std::map<uint32_t, can_message> &can_db)
{
  char frameName[512], sender[512];
	char signalName[512],signedState, unit[512], receiverList[512];
	int startBit=0,signalLength=0,byteOrder=0;
	float factor=0., offset=0., min=0., max=0.;
  char mux[4];
  int muxId = 0;
	uint32_t frameId=0, len;

  std::string line;

  while(std::getline(fp, line)) {
		if(std::sscanf(line.c_str()," BO_ %d %s %d %s",&frameId,frameName,&len,sender) == 4)
		{
      can_db[frameId] = {frameId, std::string{frameName}};
		}
		else if(std::sscanf(line.c_str()," SG_ %s : %d|%d@%d%c (%f,%f) [%f|%f] %s %s",signalName, &startBit, &signalLength,&byteOrder, &signedState, &factor, &offset, &min, &max, unit, receiverList ) > 5)
		{
      if (byteOrder == 0) {
        int pos = 7 - (startBit % 8) + (signalLength - 1);
        if (pos < 8) {
          startBit = startBit - signalLength + 1;
        } else {
          int cpos = 7 - (pos % 8);
          int bytes = (int)(pos / 8);
          startBit = cpos + (bytes * 8) + (int)(startBit/8) * 8;
        }
      }

      auto &message = can_db[frameId];
			message.signals.push_back({std::string{signalName}, startBit, signalLength, (byteOrder == 0), (signedState == '-'), factor, offset, min, max, std::string{unit}, std::string{receiverList}, 0, 0});
		}
		else if(std::sscanf(line.c_str()," SG_ %s %s : %d|%d@%d%c (%f,%f) [%f|%f] %s %s",signalName, mux, &startBit, &signalLength,&byteOrder, &signedState, &factor, &offset, &min, &max, unit, receiverList ) > 5)
		{
      if (byteOrder == 0) {
        int pos = 7 - (startBit % 8) + (signalLength - 1);
        if (pos < 8) {
          startBit = startBit - signalLength + 1;
        } else {
          int cpos = 7 - (pos % 8);
          int bytes = (int)(pos / 8);
          startBit = cpos + (bytes * 8) + (int)(startBit/8) * 8;
        }
      }
      auto &message = can_db[frameId];
      if(mux[0] == 'M') {
        message.signals.push_back({std::string{signalName}, startBit, signalLength, (byteOrder == 0), (signedState == '-'), factor, offset, min, max, std::string{unit}, std::string{receiverList}, 1, 0});
        message.isMultiplexed = 1;
      } else if(mux[0] == 'm') {
        std::sscanf(mux, "m%d", &muxId);
        message.signals.push_back({std::string{signalName}, startBit, signalLength, (byteOrder == 0), (signedState == '-'), factor, offset, min, max, std::string{unit}, std::string{receiverList}, 2, (uint8_t)muxId});
        message.isMultiplexed = 1;
      }
		}
  }
}