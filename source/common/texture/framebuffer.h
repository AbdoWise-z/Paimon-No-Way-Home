//
// Created by xAbdoMo on 5/1/2024.
//

#ifndef GFX_LAB_FRAMEBUFFER_H
#define GFX_LAB_FRAMEBUFFER_H


#include "glm/vec2.hpp"
#include "texture2d.hpp"
#include <vector>

namespace our{
    class Framebuffer {
    private:
        std::vector<Texture2D*> color;
        Texture2D* depth = nullptr;
        glm::ivec2 size;
        GLuint id;
    public:
        Framebuffer(glm::ivec2);
        void addColorTexture(GLenum format);
        void addDepthTexture(GLenum format);

        bool bind() const;
        void unbind() const;

        Texture2D* getColorTexture(int index = 0);
        Texture2D* getDepthTexture();

        int getColorTexturesCount();

        ~Framebuffer();
    };
}


#endif //GFX_LAB_FRAMEBUFFER_H
