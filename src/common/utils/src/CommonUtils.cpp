
#include "CommonUtils.hpp"

/* System defines */
#include <linux/if_ether.h>

#include <vector>
#include <regex>

int CommonUtils::getMacAddress(std::string &macaddr, uint8_t *address)
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