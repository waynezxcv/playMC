

#include "Semaphore.hpp"

using namespace GLL;


Semaphore::Semaphore(unsigned int count) :
count(count) {
}


Semaphore::~Semaphore() {
}



void Semaphore::wait() {
    std::unique_lock<std::mutex> unique_lock(m_mutex);
    -- count;
    while (m_count < 0) {
        m_condition_variable.wait(unique_lock);
    }
}

void Semaphore::signal() {
    std::lock_guard<std::mutex> lg(m_mutex);
    if (++ count < 1) {
        m_condition_variable.notify_one();
    }
}


