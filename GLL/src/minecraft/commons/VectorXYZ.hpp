//
//  VectorXYZ.hpp
//  GLL
//
//  Created by 刘微 on 2019/8/10.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#ifndef VectorXYZ_hpp
#define VectorXYZ_hpp

#include <stdio.h>
#include <stdio.h>
#include <functional>



struct VectorXYZ {
    int x, y , z;
};

bool operator ==(const VectorXYZ& left, const VectorXYZ& right) noexcept;


namespace std {
    template<>
    struct hash<VectorXYZ> {
        size_t operator()(const VectorXYZ& vect) const noexcept {
            std::hash<decltype(vect.x)> hasher;
            auto hash1 = hasher(vect.x);
            auto hash2 = hasher(vect.z);
            auto hash3 = hasher(vect.y);
            return (int)std::hash<decltype(vect.x)>{}((int)(hash1 ^ hash2 ^ hash3) >> 3);
        }
    };
}




#endif /* VectorXYZ_hpp */
