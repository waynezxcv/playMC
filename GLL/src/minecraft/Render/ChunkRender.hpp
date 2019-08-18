
#ifndef ChunkReneder_hpp
#define ChunkReneder_hpp

#include <stdio.h>
#include <vector>
#include "Worker.hpp"
#include "Render.hpp"
#include "Shader.hpp"
#include "Chunk.hpp"
#include "WorldMapGenerator.hpp"
#include "InstanceMeshDrawable.hpp"


namespace GLL {
    class ChunkManager;
}

namespace GLL {
    class ChunkRender : public Render {
    public:
        ChunkRender(const std::string& vertexShaderName);
        ~ChunkRender();
    public:        
        void draw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer, std::shared_ptr<InstanceMeshDrawable> drawable);
    private:
        void renderInit() override;
        Shader shader;
    };
}


#endif /* ChunkReneder_hpp */
