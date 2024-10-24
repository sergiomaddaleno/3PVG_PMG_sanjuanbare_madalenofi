#include "..\include\job_sistem.h"

JobSystem::JobSystem()
{

    unsigned int threads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this] { worker(); });
    }

}

JobSystem::~JobSystem()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    stop_ = true;
    
    condition_.notify_all(); 
    for (std::thread& worker : workers_) {
        worker.join();
    }
}

void JobSystem::worker()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this] { return !tasks_.empty() || stop_; });
            if (stop_ && tasks_.empty()) return;
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();
    }
}


