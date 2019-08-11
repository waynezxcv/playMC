
#include "ChunkRender.hpp"

using namespace GLL;

ChunkRender::ChunkRender(const std::string& vertexShaderName) :
shader(Shader(vertexShaderName, "chunk.frag")) {
    renderInit();
}

void ChunkRender::renderInit() {
    shader.compile();
}

ChunkRender::~ChunkRender() {
    shader.deleteProgramHandle();
}

void ChunkRender::addInstanceMesh(std::shared_ptr<InstanceMeshDrawable> instanceMesh) {
    this -> instanceMeshes.emplace_back(std::move(instanceMesh));
}


void ChunkRender::clear() {
    this -> instanceMeshes.clear();
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
    for (auto mesh : instanceMeshes) {
        mesh -> instanceDraw(camera, frameBuffer);
    }
    this -> clear();
}

