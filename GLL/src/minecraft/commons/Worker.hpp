
#ifndef Worker_hpp
#define Worker_hpp

#include <stdio.h>
#include <future>
#include <type_traits>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>


namespace GLL {
    class Worker {
    public:
        Worker() : stop(false), thread(nullptr) {
            thread = new std::thread([this](){
                for(;;) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    
                    task();
                }
            });
        }
        
        template <class F, class ... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
            using return_type = typename std::result_of<F(Args...)>::type;
            auto task = std::make_shared< std::packaged_task<return_type()> >(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            std::future<return_type> res = task->get_future();
            
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                if (stop) {
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                }
                tasks.emplace([task](){ (*task)(); });
            }
            
            condition.notify_one();
            return res;
        }
        
        
        ~Worker() {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
            condition.notify_all();
            thread -> join();
            delete thread;
        }
        
        
    private:
        std::thread* thread;
        std::condition_variable condition;
        std::mutex queueMutex;
        std::queue< std::function<void()> > tasks;
        bool stop;
    };
}


#endif /* Worker_hpp */
