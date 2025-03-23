#include "AgentFactory.hpp"
#include "Agent.hpp"

std::shared_ptr<IAgent> AgentFactory::createCanAgent()
{
  return std::make_shared<Agent>();
}