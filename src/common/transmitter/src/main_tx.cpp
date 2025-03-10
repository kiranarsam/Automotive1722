#include <mutex>
#include <condition_variable>
#include <csignal>
#include <thread>
#include <chrono>

#include <cstring>

#include "Transmitter.hpp"

std::mutex g_mutex;
std::condition_variable g_cond_var;

bool g_ready = false;
bool g_running = true;

void handleSignal(int sig);

void handleSignal(int sig)
{
  if((sig == SIGINT) || (sig == SIGTERM)) {
    g_ready = true;
    g_running = false;
    g_cond_var.notify_one();
  }
}

frame_t can_frames[2];

int main() {

  std::string ifname {"ens160"};
  std::string macaddr {"00:50:56:b0:74:a4"};
  Transmitter transmitter{ifname, macaddr};
  transmitter.init();
  transmitter.start();

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  frame_t *frame1 = &can_frames[0];
  frame_t *frame2 = &can_frames[1];
  frame1->cc.can_id = 0x123;
  frame1->cc.len = 8;
  frame2->cc.can_id = 0x456;
  frame2->cc.len = 8;
  uint8_t payload1[8] = {0x0, 0x01, 0x02, 0x03};
  uint8_t payload2[8] = {0x4, 0x05, 0x06, 0x07};
  memcpy(frame1->cc.data, payload1, 8);
  memcpy(frame1->cc.data, payload2, 8);


  while(g_running) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if(!g_running) {
      break;
    }

    transmitter.publish(can_frames, 2U);
  }

  std::unique_lock<std::mutex> lock(g_mutex);
  g_cond_var.wait(lock, [&] { return g_ready; });
  lock.unlock();

  transmitter.stop();

  return 0;
}