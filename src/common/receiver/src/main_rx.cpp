#include <mutex>
#include <condition_variable>
#include <csignal>

#include "Receiver.hpp"

std::mutex g_mutex;
std::condition_variable g_cond_var;

bool g_ready = false;

void handleSignal(int sig);

void handleSignal(int sig)
{
  if((sig == SIGINT) || (sig == SIGTERM)) {
    g_ready = true;
    g_cond_var.notify_one();
  }
}

int main() {

  std::string ifname {"ens160"};
  std::string macaddr {"00:50:56:b0:74:a4"};
  Receiver receiver{ifname, macaddr};
  receiver.init();
  receiver.start();

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  std::unique_lock<std::mutex> lock(g_mutex);
  g_cond_var.wait(lock, [&] { return g_ready; });
  lock.unlock();

  receiver.stop();

  return 0;
}