
#include "MasterRender.hpp"


using namespace GLL;


MasterRender::MasterRender() :
skyboxRender(SkyboxRender()),
chunkRender(ChunkRender("chunk.vert")),
liquidRender(ChunkRender("water.vert")),
floraRender(ChunkRender("flora.vert"))
#if TEXT_RENDER_ENABLED
textRender(TextRender())
#endif
{
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
    
#if TEXT_RENDER_ENABLED
    this -> textRender.draw(camera, frameBuffer);
#endif
    
    std::lock_guard<std::mutex> lock(this -> instanceMeshesMutex);
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
}

std::shared_ptr<InstanceMeshDrawable> MasterRender::getInstanceMeshDrawable(const std::string& key) {
    std::lock_guard<std::mutex> lock(this -> instanceMeshesMutex);
    if (this -> instanceMeshes.find(key) != this -> instanceMeshes.end()) {
        return this -> instanceMeshes[key];
    }
    return nullptr;
}

void MasterRender::insertInstanceMeshDrawableIfNeeded(std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>>&& pair) {
    std::lock_guard<std::mutex> lock(this -> instanceMeshesMutex);
    if (this -> instanceMeshes.find(pair.first) == this -> instanceMeshes.end()) {
        this->instanceMeshes.emplace(std::move(pair));
    }
}


void MasterRender::remvoeInstanceMeshDrawableIfNeeded(std::string&& key) {
    std::lock_guard<std::mutex> lock(this -> instanceMeshesMutex);
    auto itr = this -> instanceMeshes.find(key);
    if (itr != this -> instanceMeshes.end()) {
        this -> instanceMeshes.erase(itr);
    }
}

void MasterRender::clear() {
    std::lock_guard<std::mutex> lock(this -> instanceMeshesMutex);
    this -> instanceMeshes.clear();
}

