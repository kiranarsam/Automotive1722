#include "DataManager.hpp"
#include "AgentFactory.hpp"

#include <mutex>
#include <condition_variable>
#include <csignal>
#include <iostream>

std::mutex g_mutex;
std::condition_variable g_cond_var;

bool g_ready = false;

void handleSignal(int sig);

void handleSignal(int sig)
{
  if((sig == SIGINT) || (sig == SIGTERM)) {
    std::cout << "EXITED invoked sigint or sigterm " << std::endl;
    g_ready = true;
    g_cond_var.notify_one();
  }
}

int main() {
  // Data Manager
  DataManager dm;
  // Can Agent
  auto agent = AgentFactory::createCanAgent();

  dm.registerDataObserver(agent);

  dm.start();

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  std::unique_lock<std::mutex> lock(g_mutex);
  g_cond_var.wait(lock, [&] { return g_ready; });
  lock.unlock();

  //dm.unRegisterDataObserver(agent);
  // Stop the agent
  // Stop data Manager
  dm.stop();

  return 0;
}