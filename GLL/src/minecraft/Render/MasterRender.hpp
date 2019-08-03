
#ifndef MasterRender_hpp
#define MasterRender_hpp

#include <stdio.h>
#include "Render.hpp"
#include "Shader.hpp"

#include "SkyboxRender.hpp"
#include "ChunkRender.hpp"

#if TEXT_RENDER_ENABLED
#include "TextRender.hpp"
#endif


#include "LiquidRender.hpp"
#include "FloraRender.hpp"


namespace GLL {
    class World;
    class ChunkManager;
    
    
    class MasterRender : public Render {
    public:
        
        friend ChunkManager;
        
        MasterRender(World* world);
        ~MasterRender();
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        
        ChunkRender chunkRender;
        LiquidRender liquidRender;
        FloraRender floraRender;

#if TEXT_RENDER_ENABLED
        TextRender textRender;
#endif

    private:
        SkyboxRender skyboxRender;
        World* world;
    private:
        void renderInit() override;
        void drawSubRenders(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer);
    };
}

#endif /* MasterRender_hpp */
