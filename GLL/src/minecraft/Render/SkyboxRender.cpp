

#include "SkyboxRender.hpp"
#include "Image.hpp"

static GLfloat skyboxVertices [] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};


using namespace GLL;

SkyboxRender::SkyboxRender() :
VAO(0) ,
VBO(0),
texutureHandle(0),
shader(Shader("skybox.vert","skybox.frag")),
faceImageNames(std::vector<std::string> {
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/top.jpg",
    "skybox/bottom.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg"}) {
        renderInit();
    }


SkyboxRender::SkyboxRender(std::vector<std::string> faceImageNames) :
VAO(0) ,
VBO(0),
texutureHandle(0),
shader(Shader("skybox.vert","skybox.frag")),
faceImageNames(faceImageNames) {
    renderInit();
}



SkyboxRender::~SkyboxRender() {
    Render::~Render();
    glDeleteTextures(1, &texutureHandle);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    shader.deleteProgramHandle();
    faceImageNames.empty();
}


void SkyboxRender::renderInit() {
    
    this -> shader.compile();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *)0);
    glEnableVertexAttribArray(0);
    
    
    glGenTextures(1, &texutureHandle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texutureHandle);
    
    
    int i = 0;
    for (std::vector<std::string>::iterator it = faceImageNames.begin(); it != faceImageNames.end(); it ++) {
        std::string fileName = *it;
        GLL::Image image(fileName);
        if (image.getImageData() != nullptr) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB,
                         image.getWidth(),
                         image.getHeight(),
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         image.getImageData());
        }
        else {
            std::cerr<<"cube image load failed ..."<<std::endl;
        }
        i ++;
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void SkyboxRender::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    shader.use();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    shader.setUniformInt("skybox", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texutureHandle);
    // projection
    GLint projectionLocaton = glGetUniformLocation(shader.getProgramHandle(), "projection");
    glUniformMatrix4fv(projectionLocaton, 1, GL_FALSE, glm::value_ptr(camera -> getProjectionMatrix()));
    // view
    GLint viewLocation = glGetUniformLocation(shader.getProgramHandle(), "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(camera -> getViewMatrix()));
    // draw
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
