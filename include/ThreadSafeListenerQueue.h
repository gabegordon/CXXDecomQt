#pragma once

#include <queue>
#include <memory>
#include <mutex>
#ifdef __MINGW32__
#include <mingw.mutex.h>
#include <mingw.condition_variable.h>
#else
#include <condition_variable>
#endif
#include "DataTypes.h"

class ThreadSafeListenerQueue
{
  public:
  ThreadSafeListenerQueue() :
    m_active(true),
    q(),
    queueLock(),
    c()
    {}

    ~ThreadSafeListenerQueue() {}

    void push(std::unique_ptr<DataTypes::Packet> element);

    std::unique_ptr<DataTypes::Packet> listen(uint32_t& retVal);

    void setInactive();

  private:
    bool m_active;
    std::queue<std::unique_ptr<DataTypes::Packet>> q;
    mutable std::mutex queueLock;
    std::condition_variable c;
};
