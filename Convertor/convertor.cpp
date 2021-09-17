#include "convertor.h"
#include <unistd.h>

Convertor::Convertor(unsigned maxQueueSize) :
    maxSize_(!maxQueueSize ? 1 : maxQueueSize) {
}

void Convertor::run(const std::vector<std::string>& fileNames) {
    std::thread producer([&fileNames, this]()mutable{
        producerProc(*this, fileNames);
    });
    std::thread consumer([this]()mutable{consumerProc(*this);});
    //    std::thread producer(producerProc, file_names, std::ref(*this));
    producer.join();
    consumer.join();
    terminate_ = false;
}

void Convertor::consumerProc(Convertor& conv) {
    std::unique_lock<std::mutex> lock(conv.m_);
    while(!conv.terminate_ || !conv.queue_.empty()) {
        if(conv.queue_.empty()) {
            std::cout << "Queue empty: (pop) " << std::endl;
            conv.cv_.wait(lock);
            std::cout << "Start wait: (pop) " << std::endl;
            continue;
        }
        std::cout << "Reading: (pop) " << std::endl;
        std::unique_ptr<TiXmlDocument> el = std::move(conv.queue_.front());
        conv.queue_.pop();
        std::cout << "Queue size now (pop): " << conv.queue_.size() << std::endl;
        lock.unlock();
        sleep(5);
        JsonWriter jw;
        jw.writeJson(std::move(el));
        lock.lock();
    }
}

void Convertor::producerProc(Convertor& conv, const std::vector<std::string>& files) {
    for(const auto &fileName : files) {
        std::cout << "Try to write: (push) " << std::endl;
        XmlParser parser(fileName);
        std::unique_lock<std::mutex> lock(conv.m_);
        std::cout << "Write: (push) " << std::endl;
        conv.cv_.wait(lock, [&conv](){return conv.queue_.size() < conv.maxSize_;});
        conv.queue_.push(std::make_unique<TiXmlDocument>(parser.getDoc()));
        std::cout << "Queue size now: (push) " << conv.queue_.size() << std::endl;
        conv.cv_.notify_one();
    }
    std::lock_guard<std::mutex> lk(conv.m_);
    conv.terminate_ = true;
    conv.cv_.notify_one();
}
