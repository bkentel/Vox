#pragma once
#ifndef VOX_UTIL_BLOCKING_QUEUE_HPP
#define VOX_UTIL_BLOCKING_QUEUE_HPP

#include <boost/thread.hpp>
#include <queue>

namespace vox {
    namespace util {

    template <typename T>
    class BlockingQueue : private boost::noncopyable {
    public:
        template <typename U>
        void enqueue(U&& value) {
            boost::lock_guard<boost::mutex> lock(mutex_);

            queue_.push(std::forward<U>(value));

            isEmptyCond_.notify_one();
        }
        
        T dequeue() {
            boost::unique_lock<boost::mutex> lock(mutex_);

            while (queue_.empty()) {
                isEmptyCond_.wait(lock);
            }

            auto const result = queue_.front();
            queue_.pop();

            return result;
        }

        bool isEmpty() const {
            return queue_.empty();
        }

        unsigned size() const {
            return queue_.size();
        }
    private:
        boost::mutex              mutex_;
        boost::condition_variable isEmptyCond_;
        std::queue<T>             queue_;
    };


    } //namespace util
} //namespace vox


#endif //VOX_UTIL_BLOCKING_QUEUE_HPP