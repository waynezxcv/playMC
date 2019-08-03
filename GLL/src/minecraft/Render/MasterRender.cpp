
#include "MasterRender.hpp"
#include "World.hpp"

using namespace GLL;


MasterRender::MasterRender(World* world) :
skyboxRender(SkyboxRender()),
chunkRender(ChunkRender()),
liquidRender(LiquidRender()),
floraRender(FloraRender()),
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

void MasterRender::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    // 先画到framebuffer的纹理上
    frameBuffer -> activeFrameBuffer();
    // 绘制
    this -> drawSubRenders(camera, frameBuffer);
    // 再将frameBuffer的纹理绘制到当前默认的framebuffer上
    FramebufferManager::sharedInstance() -> presentFrameBufferToScreen(frameBuffer);
}

void MasterRender::drawSubRenders(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    this -> skyboxRender.draw(camera, frameBuffer);
    this -> chunkRender.draw(camera, frameBuffer);
    this -> liquidRender.draw(camera, frameBuffer);
    this -> floraRender.draw(camera, frameBuffer);
#if TEXT_RENDER_ENABLED
    this -> textRender.draw(camera, frameBuffer);
#endif
}

