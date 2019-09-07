

#ifndef Frustum_hpp
#define Frustum_hpp

#include <stdio.h>
#include <array>

namespace GLL {
    
    class AABB {
        
    public:
        AABB(const glm::vec3& dim) : dimensions  (dim) {};
        
        AABB& operator=(const AABB& rhs) {
            this -> position = rhs.position;
            this -> dimensions = rhs.dimensions;
            return *this;
        }
        
        void update(const glm::vec3& location) {
            position = location;
        }
        
        glm::vec3 getVN(const glm::vec3& normal)  const {
            glm::vec3 res = position;
            if (normal.x < 0) {
                res.x += dimensions.x;
            }
            if (normal.y < 0) {
                res.y += dimensions.y;
            }
            if (normal.z < 0) {
                res.z += dimensions.z;
            }
            return res;
        }
        
        glm::vec3 getVP(const glm::vec3& normal) const {
            glm::vec3 res = position;
            
            if (normal.x > 0) {
                res.x += dimensions.x;
            }
            if (normal.y > 0) {
                res.y += dimensions.y;
            }
            if (normal.z > 0) {
                res.z += dimensions.z;
            }
            
            return res;
        }
        
    private:
        glm::vec3 position;
        glm::vec3 dimensions;
    };
    
    
    struct Plane {
        float distanceToPoint(const glm::vec3& point) const ;
        float distanceToOrigin;
        glm::vec3 normal;
    };
    
    class ViewFrustum {
    public:
        void update(const glm::mat4& projViewMatrix) noexcept;
        
        bool isBoxInFrustum(const AABB& box) const noexcept;
        
    private:
        std::array<Plane, 6> m_planes;
    };
    
    
}

#endif /* Frustum_hpp */
