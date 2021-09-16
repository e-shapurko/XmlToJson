#ifndef QUEUE_H
#define QUEUE_H

#include <fstream>
#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

template <typename T>
class NodesQueue {
public:
    NodesQueue(uint32_t queueSize): queueSize_(queueSize){}

    void push(std::shared_ptr<T> pack){
        std::unique_lock<std::mutex> lock(m_);
        condQueueFull_.wait(lock, [&](){return nodesQueue_.size() < queueSize_;});
        nodesQueue_.push(pack);
        condQueueEmpty_.notify_all();
    }

    std::shared_ptr<T> pop(){
        std::unique_lock<std::mutex> lockRead(read_);
        condQueueEmpty_.wait(lockRead, [&](){return nodesQueue_.size() > 0;});
        std::unique_lock<std::mutex> lock(m_);
        std::shared_ptr<T> res;
        if (!nodesQueue_.empty()) {
            res = nodesQueue_.front();
            nodesQueue_.pop();
        }
        condQueueFull_.notify_all();
        return res;
    }
private:
    uint32_t queueSize_;
    std::queue<std::shared_ptr<T>> nodesQueue_;
    std::mutex m_;
    std::mutex read_;
    std::condition_variable condQueueFull_;
    std::condition_variable condQueueEmpty_;
};

#endif // QUEUE_H
