#include "convertor.h"

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
            conv.cv_.wait(lock);
            continue;
        }
        auto el = std::move(conv.queue_.front());
        conv.queue_.pop();
        conv.cv_.notify_one();
        lock.unlock();
        JsonWriter jw;
        jw.writeJson(std::move(el));
        lock.lock();
    }
}

void Convertor::producerProc(Convertor& conv, const std::vector<std::string>& files) {
    for(const auto &fileName : files) {
        XmlParser parser(fileName);
        std::unique_lock<std::mutex> lock(conv.m_);
        conv.cv_.wait(lock, [&conv](){return conv.queue_.size() < conv.maxSize_;});
        conv.queue_.push(std::make_unique<TiXmlDocument>(parser.getDoc()));
        conv.cv_.notify_one();
    }
    std::lock_guard<std::mutex> lock(conv.m_);
    conv.terminate_ = true;
    conv.cv_.notify_one();
}
