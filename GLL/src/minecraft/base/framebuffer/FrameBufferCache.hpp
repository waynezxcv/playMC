

#ifndef FrameBufferCache_hpp
#define FrameBufferCache_hpp

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include "FrameBuffer.hpp"

namespace GLL {
    
    
    
    class FrameBufferCacheItem {
    public:
        FrameBufferCacheItem();
        ~FrameBufferCacheItem();
        void lockForRead();
        void unlockForRead();
        std::vector<std::shared_ptr<FrameBuffer>> frameArray;
        
    private:
        pthread_mutex_t lock;
    };
    
    
    class FrameBufferCache {
        
    public:
        
        static FrameBufferCache* sharedInstance() {
            static FrameBufferCache instance;
            return &instance;
        }
        
        void returnFramebufferToCache(std::shared_ptr<FrameBuffer> frameBuffer);
        std::shared_ptr<FrameBuffer> fetchFrameBuffer(glm::vec2 size, TextureParameterOptions textureOptions);
        
        void clearAllCaches();
        
    private:
        
        FrameBufferCache();
        ~FrameBufferCache();
        
        std::string makeHashString(glm::vec2 size, TextureParameterOptions textureOptions) const;
        std::map<std::string, std::shared_ptr<FrameBufferCacheItem>> frameBufferCacheMap;
        
        GLuint totalBufferSize;
        void updateTotalBufferSize();
        pthread_mutex_t lock;
        
    };
}

#endif /* FrameBufferCache_hpp */
