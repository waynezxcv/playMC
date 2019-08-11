

#ifndef InstanceMeshCollection_hpp
#define InstanceMeshCollection_hpp

#include <stdio.h>
#include <vector>
#include "InstanceMeshDrawable.hpp"



namespace GLL {
    struct InstanceMeshCollection {
    public:
        std::vector<std::shared_ptr<InstanceMeshDrawable>> chunks;
        std::vector<std::shared_ptr<InstanceMeshDrawable>> liquids;
        std::vector<std::shared_ptr<InstanceMeshDrawable>> floras;
        
        void clear() {
            chunks.clear();
            liquids.clear();
            floras.clear();
        }
    };
}


#endif /* InstanceMeshCollection_hpp */
