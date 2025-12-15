#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace graphgl {

class Shader;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Delete copy constructor and assignment
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Initialize renderer
    void initialize();

    // Clear the screen
    void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;

    // Set viewport
    void setViewport(int x, int y, int width, int height) const;

    // Get viewport dimensions
    glm::ivec2 getViewportSize() const { return viewportSize_; }

private:
    glm::ivec2 viewportSize_;
    bool initialized_;
};

} // namespace graphgl

