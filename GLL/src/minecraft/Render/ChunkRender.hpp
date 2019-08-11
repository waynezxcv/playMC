
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
        
        void addInstanceMesh(std::shared_ptr<InstanceMeshDrawable> instanceMesh);
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void clear();

        
    private:
        void renderInit() override;
        Shader shader;
        std::vector<std::shared_ptr<InstanceMeshDrawable>> instanceMeshes;
    };
}


#endif /* ChunkReneder_hpp */
