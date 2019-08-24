

#include "RandomSingleton.hpp"

using namespace GLL;

RandomSingleton& RandomSingleton::sharedInstance() {
    static RandomSingleton instance;
    return instance;
}

RandomSingleton::RandomSingleton() {
    randomEngine.seed((int)std::time(nullptr));
    for (int i = 0; i < 5; i++)
        intInRange(i, i * 5);
}
