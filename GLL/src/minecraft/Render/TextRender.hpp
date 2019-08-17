
#if TEXT_RENDER_ENABLED

#ifndef TextRender_hpp
#define TextRender_hpp


#include <stdio.h>
#include <map>
#include "Render.hpp"
#include "Shader.hpp"

#include <ft2build.h>
#include <freetype/freetype.h>


namespace GLL {
    class TextRender : public Render {
        
        struct Character {
            GLuint TextureID;
            glm::ivec2 Size;
            glm::ivec2 Bearing;
            GLuint Advance;
        };
        
    public:
        TextRender();
        ~TextRender();
        
        void renderInit() override;
        void draw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void setText(const std::string& text);
        
    private:
        Shader shader;
        GLuint VAO,VBO = 0;
        GLuint texutureHandle = 0;
        std::map<GLchar, Character> Characters;
        void renderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
        std::string text;
    };
}

#endif /* TextRender_hpp */


#endif
