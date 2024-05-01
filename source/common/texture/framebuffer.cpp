//
// Created by xAbdoMo on 5/1/2024.
//

#include "framebuffer.h"
#include "texture-utils.hpp"
#include <unordered_map>
#include <thread>

std::unordered_map<std::thread::id , std::vector<GLuint>> map;

our::Framebuffer::Framebuffer(glm::ivec2 mSize) {
    size = mSize;
    glGenFramebuffers(1, &id);
}

void our::Framebuffer::addColorTexture(GLenum format) {
    auto k = bind();
    auto colorTarget = texture_utils::empty(format, size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color.size(), GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
    color.emplace_back(colorTarget);
    if (k) unbind();
}

void our::Framebuffer::addDepthTexture(GLenum format) {
    auto k = bind();
    depth = texture_utils::empty(format,size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->getOpenGLName(), 0);
    if (k) unbind();
}

bool our::Framebuffer::bind() const {
    auto thread_id = std::this_thread::get_id();
    if (map[thread_id].empty() || map[thread_id].back() != id){
        glBindFramebuffer(GL_FRAMEBUFFER,id);
        map[thread_id].emplace_back(id);
        return true;
    }
    return false;
}

void our::Framebuffer::unbind() const {
    auto thread_id = std::this_thread::get_id();
    if (map[thread_id].back() != id){
        throw "This was not the last framebuffer to be bound";
    }
    map[thread_id].pop_back();
    GLuint bind_target = 0;
    if (!map[thread_id].empty())
        bind_target = map[thread_id].back();
    glBindFramebuffer(GL_FRAMEBUFFER,bind_target);
}

our::Texture2D* our::Framebuffer::getColorTexture(int index) {
    return color[index];
}

our::Texture2D *our::Framebuffer::getDepthTexture() {
    return depth;
}

our::Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1 , &id);
    for (auto k : color){
        delete k;
    }
    delete depth;
}

int our::Framebuffer::getColorTexturesCount() {
    return color.size();
}




