
#include "VssCanExchange.hpp"

VssCanExchange::VssCanExchange() : m_dm{nullptr}, m_agent{nullptr}
{
  init();
}

VssCanExchange::~VssCanExchange()
{

}

void VssCanExchange::init()
{
  m_dm = std::make_shared<DataManager>();
  m_agent = AgentFactory::createCanAgent();
  m_dm->registerDataObserver(m_agent);
}

void VssCanExchange::start()
{
  m_agent->start();
  m_dm->start();
}

void VssCanExchange::stop()
{
  m_dm->unRegisterDataObserver(m_agent);
  m_dm->stop();
  m_agent->stop();
}
