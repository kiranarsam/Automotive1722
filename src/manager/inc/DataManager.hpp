/**
 * Copyright (c) 2025, Kiran Kumar Arsam
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *   * this list of conditions and the following disclaimer in the documentation
 *   * and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Kiran Kumar Arsam nor the names of its
 *   * contributors may be used to endorse or promote products derived from
 *   * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "IAggregator.hpp"
#include "IDataSubscriber.hpp"
#include "CanDbc.hpp"

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

class DataManager : public IDataSubscriber
{
  public:
    DataManager();
    ~DataManager();
    void init();
    void start();
    void stop();
    void registerDataObserver(std::shared_ptr<IDataObserver> data_observer);
    void unRegisterDataObserver(std::shared_ptr<IDataObserver> data_observer);
  private:

    void notify();

    void run();

    void processReceivedMessage(callback_data &msg);
    void constructMessage(callback_data &msg, CanMessage &can_msg);
    //void processTransmittedMessage()

    bool m_is_initialized;
    std::shared_ptr<IAggregator> m_can_aggregator;
    std::vector<std::shared_ptr<IDataObserver>> m_observers;

    std::queue<callback_data> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
    std::thread m_thread;

    bool m_is_running;
    std::unordered_map<uint32_t, DbCanMessage> dbcCache;
};