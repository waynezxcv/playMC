
#include "ChunkRender.hpp"

using namespace GLL;



ChunkRender::ChunkRender() :
shader(Shader("chunk.vert", "chunk.frag")) {
    renderInit();
}

void ChunkRender::renderInit() {
    shader.compile();
}

ChunkRender::~ChunkRender() {
    shader.deleteProgramHandle();
}

void ChunkRender::addDrawable(std::shared_ptr<InstanceMeshDrawable> drawable) {
    std::cout<<this->drawables.size()<<std::endl;
    
    this -> drawables.push_back(drawable);
}

void ChunkRender::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    glEnable(GL_DEPTH_TEST); // 启用深度测试
#if GL_CULL_FACE_ENABLED
    glEnable(GL_CULL_FACE); // 启用面剔除
    glCullFace(GL_BACK); // 剔除背面
    glFrontFace(GL_CCW); // 逆时针表示正面
#endif
    shader.use();
    shader.setUniformMatrix4("projection", camera -> getProjectionMatrix());
    shader.setUniformMatrix4("view", camera -> getViewMatrix());
#if INSTANCE_DRAW_ENABLED
    if (this -> drawables.empty()) {
        return;
    }
    for (auto drawable : drawables) {
        if (drawable != nullptr) {
            drawable -> instanceDraw(camera, frameBuffer);
        }
    }
#else
    
#endif
}

void ChunkRender::clearDrawables() {
    this -> drawables.clear();
}


