
#include "FrameBufferCache.hpp"
#include <sstream>
#include <memory>

using namespace GLL;


FrameBufferCacheItem::FrameBufferCacheItem() {
    pthread_mutex_init(&lock, NULL);
};

FrameBufferCacheItem::~FrameBufferCacheItem() {
    pthread_mutex_destroy(&lock);
}

void FrameBufferCacheItem::lockForRead() {
    pthread_mutex_lock(&lock);
}

void FrameBufferCacheItem::unlockForRead() {
    pthread_mutex_unlock(&lock);
}


FrameBufferCache::FrameBufferCache() {
    pthread_mutex_init(&lock, NULL);
}

FrameBufferCache::~FrameBufferCache() {
    pthread_mutex_destroy(&lock);
}


void FrameBufferCache::returnFramebufferToCache(std::shared_ptr<FrameBuffer> frameBuffer) {
    
    glm::vec2 size = frameBuffer -> getSize();
    TextureParameterOptions textureOptions = frameBuffer -> getTextureOptions();
    std::string key = makeHashString(size, textureOptions);
    
    pthread_mutex_lock(&lock);
    std::map<std::string, std::shared_ptr<FrameBufferCacheItem>> ::iterator iterator = this -> frameBufferCacheMap.find(key);
    bool found = iterator != this-> frameBufferCacheMap.end();
    pthread_mutex_unlock(&lock);
    
    if (found) {
        std::pair<std::string, std::shared_ptr<FrameBufferCacheItem>> pair = *iterator;
        std::shared_ptr<FrameBufferCacheItem> item = pair.second;
        item -> lockForRead();
        item -> frameArray.push_back(frameBuffer);
        item -> unlockForRead();
    }
    else {
        std::shared_ptr<FrameBufferCacheItem> item = std::make_shared<FrameBufferCacheItem>();
        item -> lockForRead();
        item -> frameArray.push_back(frameBuffer);
        item -> unlockForRead();
        std::pair<std::string, std::shared_ptr<FrameBufferCacheItem>> pair(key, item);
        
        pthread_mutex_lock(&lock);
        this -> frameBufferCacheMap.insert(pair);
        pthread_mutex_unlock(&lock);
    }
}


std::shared_ptr<FrameBuffer> FrameBufferCache::fetchFrameBuffer(glm::vec2 size, TextureParameterOptions textureOptions) {
    
    std::string key = makeHashString(size, textureOptions);
    pthread_mutex_lock(&lock);
    std::map<std::string, std::shared_ptr<FrameBufferCacheItem>>::iterator iterator = this -> frameBufferCacheMap.find(key);
    bool found = iterator != this-> frameBufferCacheMap.end();
    pthread_mutex_unlock(&lock);
    
    if (found) {
        std::pair<std::string, std::shared_ptr<FrameBufferCacheItem>> pair = *iterator;
        std::shared_ptr<FrameBufferCacheItem> item = pair.second;
        if (item -> frameArray.size() > 0) {
            item -> lockForRead();
            std::shared_ptr<FrameBuffer> frameBufferPtr = item -> frameArray.back();
            item -> frameArray.pop_back();
            item -> unlockForRead();
            return frameBufferPtr;
        }
        else {
            std::shared_ptr<FrameBuffer> frameBufferPtr = std::make_shared<FrameBuffer>(size, textureOptions);
            return frameBufferPtr;
        }
    }
    else {
        std::shared_ptr<FrameBuffer> frameBufferPtr = std::make_shared<FrameBuffer>(size, textureOptions);
        return frameBufferPtr;
    }
}


void FrameBufferCache::updateTotalBufferSize() {
    unsigned int count = 0;
    pthread_mutex_lock(&lock);
    for (auto iterator = this -> frameBufferCacheMap.begin(); iterator != this -> frameBufferCacheMap.end(); iterator ++) {
        std::pair<std::string, std::shared_ptr<FrameBufferCacheItem>> pair = *iterator;
        std::shared_ptr<FrameBufferCacheItem> item = pair.second;
        item -> lockForRead();
        count += item -> frameArray.size();
        item -> unlockForRead();
    }
    pthread_mutex_unlock(&lock);
    this -> totalBufferSize = count;
    std::cout<<"total fbo cache count :"<<count<<std::endl;
}

void FrameBufferCache::clearAllCaches() {
    pthread_mutex_lock(&lock);
    for (auto iterator = this -> frameBufferCacheMap.begin(); iterator != this -> frameBufferCacheMap.end(); ) {
        std::pair<std::string, std::shared_ptr<FrameBufferCacheItem>> pair = *iterator;
        std::shared_ptr<FrameBufferCacheItem> item = pair.second;
        item -> lockForRead();
        for (auto it = item -> frameArray.begin(); it != item -> frameArray.end(); ) {
            it = item -> frameArray.erase(it);
        }
        item -> unlockForRead();
        iterator = this -> frameBufferCacheMap.erase(iterator);
    }
    pthread_mutex_unlock(&lock);
    std::cout<<"frame buffer cache clear .. "<<std::endl;
    this -> updateTotalBufferSize();
}

std::string FrameBufferCache::makeHashString(glm::vec2 size, TextureParameterOptions textureOptions) const {
    std::stringstream stream;
    std::string string;
    stream<<size.x;
    stream<<size.y;
    stream<<textureOptions.minFilter;
    stream<<textureOptions.magFilter;
    stream<<textureOptions.wrapS;
    stream<<textureOptions.wrapT;
    stream<<textureOptions.wrapR;
    stream<<textureOptions.internalFormat;
    stream<<textureOptions.format;
    stream<<textureOptions.type;
    stream>>string;
    return string;
}
