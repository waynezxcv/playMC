

#include "FramebufferManager.hpp"


static GLfloat quadVertices[] = {
    // positions // texture coords
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top left
};


static GLuint quadIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

using namespace GLL;

void FramebufferManager::setupQuad() {
    quadShader.compile();
    // gen
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);
    
    // bind & buffer data
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 5, (void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void FramebufferManager::presentFrameBufferToScreen(std::shared_ptr<FrameBuffer> frameBuffer) {
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    quadShader.use();
    glBindVertexArray(quadVAO);
    
    glActiveTexture(GL_TEXTURE31);
    glBindTexture(GL_TEXTURE_2D, frameBuffer -> getFrameBufferTextrueHandle());
    glUniform1i(glGetUniformLocation(quadShader.getProgramHandle(), "texSample"), 31);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
}
