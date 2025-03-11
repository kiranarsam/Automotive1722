
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