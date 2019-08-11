//
//  VectorXYZ.cpp
//  GLL
//
//  Created by 刘微 on 2019/8/10.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#include "VectorXYZ.hpp"


bool operator ==(const VectorXYZ& left, const VectorXYZ& right) noexcept {
    return  (left.x == right.x) && (left.z == right.z) && (left.y == right.y);
}
