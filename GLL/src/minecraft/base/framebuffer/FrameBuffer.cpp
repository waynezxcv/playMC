

#include "FrameBuffer.hpp"
#include "FrameBufferCache.hpp"


using namespace GLL;


FrameBuffer::FrameBuffer() :
size(glm::vec2{WINDOW_WIDTH, WINDOW_HEIGHT}),
textureOptions(FrameBuffer::getDefaultTextureOptions()),
frameBufferHandle(0),
textureHandle(0),
colorRenderBufferHandle1(0),
colorRenderBufferHandle2(0),
colorRenderBufferHandle3(0),
depthStencilRenderBufferHandle(0),
framebufferReferenceCount(0)
{
    setup();
}

FrameBuffer::FrameBuffer(glm::vec2 frameSize, TextureParameterOptions options, bool onlyTexture) :
size(frameSize),
textureOptions(options),
frameBufferHandle(0),
textureHandle(0),
colorRenderBufferHandle1(0),
colorRenderBufferHandle2(0),
colorRenderBufferHandle3(0),
depthStencilRenderBufferHandle(0),
framebufferReferenceCount(0)
{
    setup();
}


void FrameBuffer::setup() {
    
    glGenFramebuffers(1, &frameBufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
    
    // texture
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    // 我们将维度设置为了屏幕大小（尽管这不是必须的），并且我们给纹理的data参数传递了NULL。对于这个纹理，我们仅仅分配了内存而没有填充它
    // 填充这个纹理将会在我们渲染到帧缓冲之后来进行。同样注意我们并不关心环绕方式或多级渐远纹理，我们在大多数情况下都不会需要它们。
    glTexImage2D(GL_TEXTURE_2D, 0, textureOptions.internalFormat, size.x, size.y, 0, textureOptions.format, textureOptions.type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureOptions.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureOptions.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureOptions.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureOptions.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, textureOptions.wrapR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);
    /*
     数据是原生的格式 当写入或者复制它的数据到其它缓冲中时是非常快的。
     后引入的framebuffer附件类型，之前只能是纹理
     能够使用glReadPixels来读取它，这会从当前绑定的帧缓冲，而不是附件本身，中返回特定区域的像素。
     */
    // depth and stencil render buffer
    glGenRenderbuffers(1, &depthStencilRenderBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, depthStencilRenderBufferHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRenderBufferHandle);
    
    // check status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw "the frame buffer is not completed .. ";
        return;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    // 将framebuffer绑回默认的帧缓冲对象，前面都是在执行离屏渲染，否则无法显示出来
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::creatColorRenderBufferForColorAttach1() {
    glGenRenderbuffers(1, &colorRenderBufferHandle1);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBufferHandle1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, colorRenderBufferHandle1);
}


void FrameBuffer::creatColorRenderBufferForColorAttach2() {
    glGenRenderbuffers(1, &colorRenderBufferHandle2);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBufferHandle2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, colorRenderBufferHandle2);
}


void FrameBuffer::creatColorRenderBufferForColorAttach3() {
    glGenRenderbuffers(1, &colorRenderBufferHandle3);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBufferHandle3);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_RENDERBUFFER, colorRenderBufferHandle3);
}


void FrameBuffer::activeFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
    // 清除上一帧framebuffer上的数据
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void FrameBuffer::destoryFrameBuffer() {
    glDeleteFramebuffers(1, &frameBufferHandle);
    glDeleteRenderbuffers(1, &depthStencilRenderBufferHandle);
    glDeleteTextures(1, &textureHandle);
    frameBufferHandle = 0;
    depthStencilRenderBufferHandle = 0;
    textureHandle = 0;
}

void FrameBuffer::lock() {
    framebufferReferenceCount ++;
}

void FrameBuffer::unlock() {
    framebufferReferenceCount--;
    if (framebufferReferenceCount == 0) {
        std::shared_ptr<FrameBuffer> shared = shared_from_this();
        FrameBufferCache::sharedInstance() -> returnFramebufferToCache(shared);
    }
}


GLuint FrameBuffer::getFrameBufferTextrueHandle() const {
    return textureHandle;
}


glm::vec2 FrameBuffer::getSize() const {
    return this -> size;
}


TextureParameterOptions FrameBuffer::getTextureOptions() const {
    return this -> textureOptions;
}


FrameBuffer::~FrameBuffer() {
    destoryFrameBuffer();
    std::cout<<"Destory Frame Buffer Object!"<<std::endl;
};

GLuint FrameBuffer::getFrameBufferHandle() const  {
    return this -> frameBufferHandle;
}

