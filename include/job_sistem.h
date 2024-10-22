#ifndef JBS
#define JBS

#include <functional>
#include <vector>

#include <mutex>
#include <queue>
#include <memory>
#include <functional>
#include <thread>

class JobSystem {
public:
    JobSystem();  

    void AddNewFunction(std::function<void()> new_function) {
        tasks.push_back(new_function);  
    }

    void worker() {

        while (true) {

            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this] { return !tasks.empty() || stop_; });
                if (stop_ && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();

            } // Es importante soltar el lock antes de empezar la tarea.
            task();
        }
    }

private:

    std::vector<std::function<void()>> tasks; 
    std::vector<std::thread> threads;

    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

#endif JBS