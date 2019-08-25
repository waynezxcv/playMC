
#ifndef ChunkDefine_hpp
#define ChunkDefine_hpp

#include <stdio.h>

namespace {
    constexpr GLint CHUNK_SIZE = 16;
    constexpr GLint CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr GLint CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    constexpr GLuint RENDER_DISTANCE = 16;
    constexpr GLuint UNRENDER_DISTANCE = 16 * 2;

    constexpr GLuint WATER_LEVEL = 64;
    constexpr GLuint CHUNK_LIMIT_SIZE = 1000;
}

#endif /* ChunkDefine_hpp */
