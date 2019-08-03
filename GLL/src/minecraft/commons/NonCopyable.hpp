
#ifndef NonCopyable_hpp
#define NonCopyable_hpp

#include <stdio.h>

namespace GLL {
    class NonCopyable {
    public:
        NonCopyable() = default;
    private:
        NonCopyable(const NonCopyable& rhs) = delete;
        NonCopyable& operator=(const NonCopyable& rhs) = delete;
    };
}

#endif /* NonCopyable_hpp */
