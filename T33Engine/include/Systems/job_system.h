/**
 * @file job_system.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __JOB_SYSTEM__
#define __JOB_SYSTEM__

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

 /**
  * @class JobSystem
  * @brief Manages a queue of asynchronous tasks and executes them in a separate thread.
  */
class JobSystem {
public:
    JobSystem();
    ~JobSystem();

    template <class F, class... Args>
    auto Add_Task(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type> {

        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return res;
    }

private:
    void WorkerThread();

    std::vector<std::thread> workers_;
    
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;

    bool done_;
};

#endif __JOB_SYSTEM__


