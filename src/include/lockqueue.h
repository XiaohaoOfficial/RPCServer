#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class LockQueue
{
public:
    T pop()
    {
        std::unique_lock<std::mutex> lck(_mtx);
        while(_lockQueue.empty())
        {
            _cv.wait(lck);
        }
        T data = _lockQueue.front();
        _lockQueue.pop();
        return data;
    }
    void push(T t)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        _lockQueue.push(t);
        _cv.notify_one();
    }
    int size()const
    {
        return _lockQueue.size();
    }

private:
    std::queue<T> _lockQueue;
    std::mutex _mtx;
    std::condition_variable _cv;
};