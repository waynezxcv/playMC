
#include "ChunkRender.hpp"

using namespace GLL;



ChunkRender::ChunkRender(const std::string& vertexShaderName) :
shader(Shader(vertexShaderName, "chunk.frag")) {
    renderInit();
}



void ChunkRender::renderInit() {
    pthread_mutex_init(&lock, NULL);
    shader.compile();
}

ChunkRender::~ChunkRender() {
    pthread_mutex_destroy(&lock);
    shader.deleteProgramHandle();
}

void ChunkRender::addInstanceDrawablesIfNeeded(std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair) {
    pthread_mutex_lock(&lock);
    bool found = this -> drawableMap.find(pair.first) != this -> drawableMap.end();
    if (!found) {
        this->drawableMap.insert(pair);
    }
    pthread_mutex_unlock(&lock);
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
    shader.setUniformFloat("globalTime",(float)glfwGetTime());
    
    pthread_mutex_lock(&lock);
    if (this -> drawableMap.empty()) {
        pthread_mutex_unlock(&lock);
        return;
    }
    
    for (auto it = drawableMap.begin(); it != drawableMap.end(); it ++) {
        std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair = *it;
        std::string key = pair.first;
        std::shared_ptr<InstanceMeshDrawable> drawable = pair.second;
        if (drawable != nullptr) {
            drawable->instanceDraw(camera, frameBuffer);
        }
    }
    pthread_mutex_unlock(&lock);
}

void ChunkRender::clearDrawables() {
    pthread_mutex_lock(&lock);
    this -> drawableMap.clear();
    pthread_mutex_unlock(&lock);
}
