#pragma once

#include "IAgent.hpp"
#include <memory>

class AgentFactory final
{
private:
  AgentFactory() = delete;
  ~AgentFactory() = delete;
public:
  static std::shared_ptr<IAgent> createCanAgent();
private:

};