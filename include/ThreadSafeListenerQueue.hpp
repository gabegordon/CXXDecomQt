#pragma once

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include "DataTypes.hpp"

template<class T>
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

/**
 * Pushes an element into the queue. Notifies one waiting listener.
 *
 * @param element to be pushed
 * @return N/A
 */
    void push(T&& element)
    {
        std::lock_guard<std::mutex> lock(queueLock);  // Lock mutex to prevent simultaneous push and pops
        q.push(std::move(element));  // Move into queue
        c.notify_one();  // Notify one waiting thread
    }

/**
 * Function that blocks while queue is empty on condition_variable c. Once notified pops from queue and acquires lock.
 *
 * @param retVal Integer passed by referenec to get return value. 1 on success. 0 on timeout.
 * @return queue value
 */
    uint32_t listen(T& queueVal)
    {
        std::unique_lock<std::mutex> lock(queueLock);  // Get mutex lock
        while (q.empty())  // While loop checking empty to account for spurious wakeups
        {
            if (c.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout)  // Wait on condition var until notified, or 1 second timeout is reached
            {
                if (!m_active && q.empty())
                {
                    return 0;
                }
            }
        }

        queueVal = std::move(q.front());  // Once notified save front queue
        q.pop();  // Then pop it
        return 1;
    }

    size_t size()
    {
        return q.size();
    }

/**
 * Sets member variable m_active to false. This causes threads to terminate once queue becomes empty.
 *
 * @return N/A
 */
    void setInactive()
    {
        std::lock_guard<std::mutex> lock(queueLock);
        m_active = false;
    }
  private:
    bool m_active;
    std::queue<T> q;
    mutable std::mutex queueLock;
    std::condition_variable c;
};
