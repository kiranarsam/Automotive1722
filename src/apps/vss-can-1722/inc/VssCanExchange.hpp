#pragma once

#include <memory>
#include "DataManager.hpp"
#include "AgentFactory.hpp"


class VssCanExchange
{
public:
  VssCanExchange();
  ~VssCanExchange();

  void start();

  void stop();

private:

  void init();

  std::shared_ptr<DataManager> m_dm;
  std::shared_ptr<IAgent> m_agent;
};