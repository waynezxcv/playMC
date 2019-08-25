

#include "VectorXYZ.hpp"


bool operator ==(const VectorXYZ& left, const VectorXYZ& right) noexcept {
    return  (left.x == right.x) && (left.z == right.z) && (left.y == right.y);
}
