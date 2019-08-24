

#ifndef FrameBuffer_hpp
#define FrameBuffer_hpp

#include <stdio.h>
#include <memory>
#include "NonCopyable.hpp"


/*
 帧缓冲对象（画板）
 可以包含颜色、深度、模板附件（夹子）
 夹子可以夹纹理、渲染缓冲对象(画布)
 */
namespace GLL {
    
    typedef struct {
        GLenum minFilter;
        GLenum magFilter;
        
        GLenum wrapS;
        GLenum wrapT;
        GLenum wrapR;
        
        GLenum internalFormat;
        GLenum format;
        GLenum type;
        
    } TextureParameterOptions;
    
    
    class FrameBuffer : public std::enable_shared_from_this<FrameBuffer> , public NonCopyable {
        // 当类被share_ptr管理，且在类的成员函数里需要把当前类对象作为参数传给其他函数时，就需要传递一个指向自身的share_ptr。
    public:
        
        static TextureParameterOptions getDefaultTextureOptions();
        
        FrameBuffer();
        ~FrameBuffer();
        
        explicit FrameBuffer(glm::vec2 frameSize = {WINDOW_WIDTH, WINDOW_HEIGHT},
        TextureParameterOptions options = FrameBuffer::getDefaultTextureOptions(),
        bool onlyTexture = false);
        
        TextureParameterOptions getTextureOptions() const;

        void lock();
        void unlock();
        void activeFrameBuffer();
        void destoryFrameBuffer();
        
        void creatColorRenderBufferForColorAttach1();
        void creatColorRenderBufferForColorAttach2();
        void creatColorRenderBufferForColorAttach3();
    public:
        glm::vec2 getSize() const;
        GLuint getFrameBufferTextrueHandle() const;
        GLuint getFrameBufferHandle() const;
    private:
        
        glm::vec2 size;
        TextureParameterOptions textureOptions;
        GLuint frameBufferHandle;
        
        GLuint textureHandle;
        GLuint depthStencilRenderBufferHandle;
        GLuint framebufferReferenceCount;
        
        
        GLuint colorRenderBufferHandle1;
        GLuint colorRenderBufferHandle2;
        GLuint colorRenderBufferHandle3;

    private:
        void setup();
        
    };
}

#endif /* FrameBuffer_hpp */
