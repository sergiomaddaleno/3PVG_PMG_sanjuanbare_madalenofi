
#include "Systems/job_system.h"
#include "utils.h"

JobSystem::JobSystem() : done_(false) {
    unsigned int thread_count = std::thread::hardware_concurrency();
    for (unsigned int i = 0; i < thread_count; ++i) {
        workers_.emplace_back(&JobSystem::WorkerThread, this);
    }
}

JobSystem::~JobSystem()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        done_ = true;
    }
    condition_.notify_all();
    for (std::thread& worker : workers_) {
        worker.join();
    }
}

void JobSystem::WorkerThread()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this] { return done_ || !tasks_.empty(); });
            if (done_ && tasks_.empty()) return;
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task(); // Ejecuta la tarea
    }
}
