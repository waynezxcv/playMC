
#ifndef Array2D_hpp
#define Array2D_hpp

#include <stdio.h>
#include <array>
#include <algorithm>
#include <mutex>

namespace GLL {
    
    template<typename T, int WIDTH>
    
    class Array2D {
        
    public:
        Array2D() {
            
        }
        
        T& get(int x, int z) {
            std::lock_guard<std::mutex> lock(mutex);
            return m_array[x * WIDTH + z];
        }
        
        const T& get(int x, int z) const {
            std::lock_guard<std::mutex> lock(mutex);
            return m_array[x * WIDTH + z];
        }
        
        T& getMaxValue() {
            std::lock_guard<std::mutex> lock(mutex);
            return *std::max_element(m_array.begin(), m_array.end());
        }
        
        void setAll(T val) {
            std::lock_guard<std::mutex> lock(mutex);
            m_array.fill(val);
        }

    private:
        std::array<T, WIDTH * WIDTH> m_array;
        mutable std::mutex mutex;
    };
    
}


#endif /* Array2D_hpp */
