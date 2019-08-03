
#ifndef ChunkReneder_hpp
#define ChunkReneder_hpp

#include <stdio.h>
#include <vector>
#include "Render.hpp"
#include "Shader.hpp"
#include "Chunk.hpp"
#include "WorldMapGenerator.hpp"
#include "ChunkManager.hpp"



namespace GLL {
    class ChunkManager;
    class ChunkRender : public Render {
        
    public:
        ChunkRender();
        ~ChunkRender();
        
        void addDrawable(std::shared_ptr<InstanceMeshDrawable> drawable);
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void clearDrawables();
        
    private:
        Shader shader;
        std::vector<std::shared_ptr<InstanceMeshDrawable>> drawables;
        void renderInit() override;
    };
}


#endif /* ChunkReneder_hpp */
