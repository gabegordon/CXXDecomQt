#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ProgressQueue
{
public:
    ProgressQueue() {}

    ~ProgressQueue() {}

    void push(const T element)
    {
        std::unique_lock<std::mutex> lock(m);
        q.push_back(element);
        c.notify_one();
        lock.unlock();
    }

    int listen(T& element)
    {
        std::unique_lock<std::mutex> lock(m);
        while(q.empty())
        {
            c.wait(lock);
        }
        element = q.front();
        q.pop();
        lock.unlock();
        return 0;
    }
private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};
