
#ifndef ChunkReneder_hpp
#define ChunkReneder_hpp

#include <stdio.h>
#include <vector>
#include "Worker.hpp"

#include "Render.hpp"
#include "Shader.hpp"
#include "Chunk.hpp"
#include "WorldMapGenerator.hpp"
#include "ChunkManager.hpp"



namespace GLL {
    class ChunkManager;
    class ChunkRender : public Render {
    public:
        ChunkRender(const std::string& vertexShaderName);
        ~ChunkRender();
        
        void addInstanceDrawablesIfNeeded(std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair);
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void clearDrawables();

    private:
        void renderInit() override;
        Shader shader;
        pthread_mutex_t lock;
        std::map<std::string, std::shared_ptr<InstanceMeshDrawable>> drawableMap;
    };
}


#endif /* ChunkReneder_hpp */
