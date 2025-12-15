#include "equation_renderer.h"
#include <glad/glad.h>
#include <iostream>

namespace graphgl {

EquationRenderer::EquationRenderer()
    : VAO_(0)
    , VBO_(0)
    , EBO_(0)
    , initialized_(false)
{
}

EquationRenderer::~EquationRenderer() {
    cleanupBuffers();
}

void EquationRenderer::initialize() {
    if (initialized_) {
        return;
    }
    setupBuffers();
    initialized_ = true;
}

void EquationRenderer::updateVertices(const std::vector<Equation>& equations, 
                                     const std::vector<Point>& points) {
    vertices_.clear();
    indices_.clear();

    size_t vertexOffset = 0;

    // Add equation vertices
    for (const auto& equation : equations) {
        if (equation.isVisible) {
            vertices_.insert(vertices_.end(), 
                           equation.vertices.begin(), 
                           equation.vertices.end());

            // Add indices with offset
            if (equation.isMesh) {
                for (unsigned int index : equation.indices) {
                    indices_.push_back(index + vertexOffset);
                }
            }
            vertexOffset += equation.vertices.size();
        }
    }

    // Add point vertices
    for (const auto& point : points) {
        vertices_.insert(vertices_.end(),
                        point.vertexData.begin(),
                        point.vertexData.end());
    }

    // Update OpenGL buffers
    if (VAO_ == 0) {
        setupBuffers();
    }

    glBindVertexArray(VAO_);

    // Update vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    if (!vertices_.empty()) {
        glBufferData(GL_ARRAY_BUFFER, 
                    vertices_.size() * sizeof(glm::vec3),
                    vertices_.data(),
                    GL_STATIC_DRAW);
    }

    // Update index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    if (!indices_.empty()) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    indices_.size() * sizeof(unsigned int),
                    indices_.data(),
                    GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

void EquationRenderer::render(const Shader& shader, bool useHeatmap, 
                             float minHeight, float maxHeight) const {
    if (VAO_ == 0 || vertices_.empty()) {
        return;
    }

    shader.use();
    shader.setBool("use_heatmap", useHeatmap);
    shader.setFloat("min_height", minHeight);
    shader.setFloat("max_height", maxHeight);

    glBindVertexArray(VAO_);

    // Draw mesh if indices exist
    if (!indices_.empty()) {
        glDrawElements(GL_TRIANGLES, 
                      static_cast<GLsizei>(indices_.size()),
                      GL_UNSIGNED_INT,
                      0);
    }

    // Draw points for remaining vertices
    size_t pointStartIndex = indices_.empty() ? 0 : vertices_.size() - indices_.size();
    if (pointStartIndex < vertices_.size()) {
        glDrawArrays(GL_POINTS,
                    static_cast<GLsizei>(pointStartIndex / 2),
                    static_cast<GLsizei>((vertices_.size() - pointStartIndex) / 2));
    }

    glBindVertexArray(0);
}

void EquationRenderer::setupBuffers() {
    cleanupBuffers();

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void EquationRenderer::cleanupBuffers() {
    if (EBO_ != 0) {
        glDeleteBuffers(1, &EBO_);
        EBO_ = 0;
    }
    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
        VBO_ = 0;
    }
    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
        VAO_ = 0;
    }
}

} // namespace graphgl

