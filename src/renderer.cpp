#include "renderer.h"
#include <iostream>

namespace graphgl {

Renderer::Renderer()
    : viewportSize_(1280, 720)
    , initialized_(false)
{
}

Renderer::~Renderer() {
    // Cleanup handled by RAII
}

void Renderer::initialize() {
    if (initialized_) {
        return;
    }

    // OpenGL state is already set up by Application class
    // This is a placeholder for future renderer-specific initialization
    
    initialized_ = true;
}

void Renderer::clear(float r, float g, float b, float a) const {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    viewportSize_ = glm::ivec2(width, height);
}

} // namespace graphgl

