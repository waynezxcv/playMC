
#include "MasterRender.hpp"
#include "World.hpp"

using namespace GLL;


MasterRender::MasterRender(World* world) :
skyboxRender(SkyboxRender()),
chunkRender(ChunkRender("chunk.vert")),
liquidRender(ChunkRender("water.vert")),
floraRender(ChunkRender("flora.vert")),
#if TEXT_RENDER_ENABLED
textRender(TextRender()),
#endif
world(world) {
    renderInit();
}

MasterRender::~MasterRender() {
}

void MasterRender::renderInit() {
}

void MasterRender::draw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    // 先画到framebuffer的纹理上
    frameBuffer -> activeFrameBuffer();
    // 绘制
    this -> drawSubRenders(camera, frameBuffer);
    // 再将frameBuffer的纹理绘制到当前默认的framebuffer上
    FramebufferManager::sharedInstance() -> presentFrameBufferToScreen(frameBuffer);
}


void MasterRender::drawSubRenders(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    this -> skyboxRender.draw(camera, frameBuffer);
    
    for (auto one : instanceMeshes) {
        BlockShaderType shaderType = one.second -> getBlockData().shaderType;
        if (shaderType == BlockShaderType_Chunck) {
            this -> chunkRender.draw(camera, frameBuffer, one.second);
        }
        else if (shaderType == BlockShaderType_Liquid) {
            this -> liquidRender.draw(camera, frameBuffer, one.second);
        }
        else if (shaderType == BlockShaderType_Flora) {
            this -> floraRender.draw(camera, frameBuffer, one.second);
        }
    }
#if TEXT_RENDER_ENABLED
    this -> textRender.draw(camera, frameBuffer);
#endif
}

std::shared_ptr<InstanceMeshDrawable> MasterRender::getInstanceMeshDrawable(const std::string& key) {
    if (this -> instanceMeshes.find(key) != this -> instanceMeshes.end()) {
        return this -> instanceMeshes[key];
    }
    return nullptr;
}

void MasterRender::insertInstanceMeshDrawableIfNeeded(std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair) {
    if (this -> instanceMeshes.find(pair.first) == this -> instanceMeshes.end()) {
        this -> instanceMeshes.insert(pair);
    }
}

void MasterRender::clear() {
    this -> instanceMeshes.clear();
}

