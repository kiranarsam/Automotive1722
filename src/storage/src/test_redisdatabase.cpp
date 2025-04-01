#include "RedisDatabase.hpp"

#include <mutex>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <chrono>
#include <sstream>

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

  RedisDatabase db_repo;
  db_repo.start();
  for(int i = 0; i <= 10; i++) {
    RedisRequestMsg req1;
    req1.type = RedisDbType::GET;
    req1.key = "key";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    db_repo.command(req1);

    RedisRequestMsg req2;
    req2.type = RedisDbType::SET;
    req2.key = "key";
    req2.value = std::to_string(i);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    db_repo.command(req2);
  }

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  std::unique_lock<std::mutex> lock(g_mutex);
  g_cond_var.wait(lock, [&] { return g_ready; });
  lock.unlock();

  //dm.unRegisterDataObserver(agent);
  // Stop the agent
  // Stop data Manager
  db_repo.stop();

  return 0;
}