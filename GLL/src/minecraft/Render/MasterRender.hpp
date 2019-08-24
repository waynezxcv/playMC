
#ifndef MasterRender_hpp
#define MasterRender_hpp

#include <stdio.h>
#include <mutex>

#include "Render.hpp"
#include "Shader.hpp"

#include "SkyboxRender.hpp"
#include "ChunkRender.hpp"

#if TEXT_RENDER_ENABLED
#include "TextRender.hpp"
#endif



namespace GLL {
    class ChunkManager;
}


namespace GLL {
    
    class MasterRender : public Render {
    public:
        
        friend ChunkManager;
        
        MasterRender();
        ~MasterRender();

        void draw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        std::shared_ptr<InstanceMeshDrawable> getInstanceMeshDrawable(const std::string& key);
        void insertInstanceMeshDrawableIfNeeded(std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>>&& pair);
        void clear();
    public:
        ChunkRender chunkRender;
        ChunkRender liquidRender;
        ChunkRender floraRender;
#if TEXT_RENDER_ENABLED
        TextRender textRender;
#endif

    private:
        SkyboxRender skyboxRender;
        std::map<std::string, std::shared_ptr<InstanceMeshDrawable>> instanceMeshes;
        std::mutex instanceMeshesMutex;
        
    private:
        void renderInit() override;
        void drawSubRenders(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer);
    };
}

#endif /* MasterRender_hpp */
