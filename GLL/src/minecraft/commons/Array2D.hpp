
#ifndef Array2D_hpp
#define Array2D_hpp

#include <stdio.h>
#include <array>
#include <algorithm>

namespace GLL {
    
    template<typename T, int WIDTH>
    
    class Array2D {
        
    public:
        
        T& get(int x, int z) {
            return m_array[x * WIDTH + z];
        }
        
        const T& get(int x, int z) const {
            return m_array[x * WIDTH + z];
        }
        
        T& getMaxValue() {
            return *std::max_element(m_array.begin(), m_array.end());
        }
        
        void setAll(T val) {
            m_array.fill(val);
        }
        

    private:
        std::array<T, WIDTH * WIDTH> m_array;
    };
    
}


#endif /* Array2D_hpp */
