
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
    class ChunkRender : public Render {
    public:
        ChunkRender(const std::string& vertexShaderName);
        ~ChunkRender();
        
    public:
        
        void addDrawableIfNeeded(std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair);
        void draw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void clear();
        
    private:
        void renderInit() override;
        Shader shader;
        std::map<std::string, std::shared_ptr<InstanceMeshDrawable>> instanceMeshes;
    };
}


#endif /* ChunkReneder_hpp */
