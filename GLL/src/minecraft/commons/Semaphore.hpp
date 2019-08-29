
#ifndef Semaphore_hpp
#define Semaphore_hpp

#include <stdio.h>
#include <mutex>
#include <condition_variable>


namespace GLL {
    
    class Semaphore {
    public:
        explicit Semaphore(unsigned int count); //用无符号数表示信号量资源
        ~Semaphore();
        
    public:
        void wait();
        void signal();
    private:
        int count; //计数器必须是有符号数
        std::mutex m_mutex;
        std::condition_variable m_condition_variable;
    };
    
    
}

#endif /* Semaphore_hpp */
