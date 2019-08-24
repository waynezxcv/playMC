
#ifndef Random_hpp
#define Random_hpp

#include <stdio.h>
#include <random>
#include <ctime>


namespace GLL {
    
    class RandomSingleton {
    public:
        static RandomSingleton& sharedInstance();
        template<typename T>
        T intInRange(T low, T high) {
            static_assert(std::is_integral<T>::value, "Not integral type!");
            std::uniform_int_distribution<T> dist(low, high);
            return dist(randomEngine);
        }
    private:
        RandomSingleton();
        std::mt19937 randomEngine;
    };

    
    template<typename REngine = std::mt19937>
    class Random {
    public:
        Random(int n = (int)std::time(nullptr)) {
            randomEngine.seed(n);
            for (int i = 0; i < 5; i++)
                intInRange(i, i * 5);
        }
        template<typename T>
        T intInRange(T low, T high) {
            static_assert(std::is_integral<T>::value, "Not integral type!");
            std::uniform_int_distribution<T> dist(low, high);
            return dist(randomEngine);
        }
        void setSeed(int seed) {
            randomEngine.seed(seed);
        }
    private:
        REngine randomEngine;
    };
        
}

#endif /* Random_hpp */
