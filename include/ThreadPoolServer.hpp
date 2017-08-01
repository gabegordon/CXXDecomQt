#pragma once

#include <thread>
#include <string>
#include <vector>
#include <unordered_map>
#include "ThreadSafeListenerQueue.hpp"
#include "DataTypes.hpp"

class ThreadPoolServer
{
  public:
  ThreadPoolServer() :
    m_queues{}
    {}

    ~ThreadPoolServer() {}

    void exec(std::unique_ptr<DataTypes::Packet> pack, const std::string& instrument);
    void join();
    void ThreadMain(ThreadSafeListenerQueue<std::unique_ptr<DataTypes::Packet>>& queue, const std::string& instrument, const uint32_t apid);

  private:
    std::unordered_map<uint32_t, ThreadSafeListenerQueue<std::unique_ptr<DataTypes::Packet>>> m_queues;
    std::vector<std::thread> m_threads;
};
