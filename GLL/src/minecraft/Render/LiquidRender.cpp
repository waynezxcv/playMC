//
//  LiquidRender.cpp
//  GLL
//
//  Created by 刘微 on 2019/7/28.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#include "LiquidRender.hpp"

using namespace GLL;

LiquidRender::LiquidRender() :
shader(Shader("water.vert", "chunk.frag")) {
    renderInit();
}


void LiquidRender::renderInit() {
    shader.compile();
}

LiquidRender::~LiquidRender() {
    shader.deleteProgramHandle();
}

void LiquidRender::addDrawable(std::shared_ptr<InstanceMeshDrawable> drawable) {
    this -> drawables.push_back(drawable);
}

void LiquidRender::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
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

void LiquidRender::clearDrawables() {
    this -> drawables.clear();
}


