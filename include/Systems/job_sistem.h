#ifndef __JOB_SYSTEM__
#define __JOB_SYSTEM__

#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <future>
#include "..\Core\mesh.h"

#include <thread>
#include <queue>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>

class JobSystem {
public:
    JobSystem() : done_(false) {
        worker_ = std::thread(&JobSystem::WorkerThread, this);
    }

    ~JobSystem() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            done_ = true;
        }
        condition_.notify_all();
        worker_.join();
    }

    template <class F>
    std::future<std::unique_ptr<Mesh>> Add_Task(F&& f) {
        using return_type = std::unique_ptr<Mesh>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));

        std::future<std::unique_ptr<Mesh>> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return res;
    }

private:
    void WorkerThread() {
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

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool done_;
    std::thread worker_; // Solo un hilo por simplicidad
};

#endif __JOB_SYSTEM__


