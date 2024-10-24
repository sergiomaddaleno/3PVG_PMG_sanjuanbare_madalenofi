#ifndef JBS
#define JBS


#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <future>
#include "..\include\mesh.h"


class JobSystem {
public:

    JobSystem();
    ~JobSystem();
    
  /*
    template<class F>
    void Add_Taks(F&& f) {
         std::unique_lock<std::mutex> lock(queue_mutex_);
         tasks_.emplace(std::forward<F>(f));        
        condition_.notify_one();
    }
    */
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
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;

    void worker();
};

#endif JBS


