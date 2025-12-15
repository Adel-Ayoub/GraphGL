#include "grid_renderer.h"
#include <glad/glad.h>
#include <vector>
#include <cmath>

namespace graphgl {

GridRenderer::GridRenderer()
    : VAO_grid_(0)
    , VBO_grid_(0)
    , VAO_axes_(0)
    , VBO_axes_(0)
    , gridVertexCount_(0)
    , axesVertexCount_(6)
    , gridLinesVisible_(true)
    , axesVisible_(true)
    , initialized_(false)
{
}

GridRenderer::~GridRenderer() {
    cleanupBuffers();
}

void GridRenderer::initialize(float gridSize, float gridSpacing) {
    if (initialized_) {
        return;
    }

    setupGridBuffers(gridSize, gridSpacing);
    setupAxesBuffers();
    initialized_ = true;
}

void GridRenderer::renderGridLines(const Shader& shader) const {
    if (!gridLinesVisible_ || VAO_grid_ == 0) {
        return;
    }

    shader.use();
    shader.setBool("use_line", true);
    shader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

    glBindVertexArray(VAO_grid_);
    glDrawArrays(GL_LINES, 0, gridVertexCount_);
    glBindVertexArray(0);

    shader.setBool("use_line", false);
}

void GridRenderer::renderAxes(const Shader& shader) const {
    if (!axesVisible_ || VAO_axes_ == 0) {
        return;
    }

    shader.use();
    shader.setBool("use_gridline", true);
    shader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

    glBindVertexArray(VAO_axes_);
    glDrawArrays(GL_LINES, 0, axesVertexCount_);
    glBindVertexArray(0);

    shader.setBool("use_gridline", false);
}

void GridRenderer::setupGridBuffers(float gridSize, float gridSpacing) {
    std::vector<float> gridVertices;

    // Generate grid lines in all three planes
    // XZ plane (horizontal)
    for (float i = -gridSize; i <= gridSize; i += gridSpacing) {
        // Lines parallel to X axis
        gridVertices.insert(gridVertices.end(), {-gridSize, 0.0f, i});
        gridVertices.insert(gridVertices.end(), {gridSize, 0.0f, i});
        
        // Lines parallel to Z axis
        gridVertices.insert(gridVertices.end(), {i, 0.0f, -gridSize});
        gridVertices.insert(gridVertices.end(), {i, 0.0f, gridSize});
    }

    // YZ plane
    for (float i = -gridSize; i <= gridSize; i += gridSpacing) {
        gridVertices.insert(gridVertices.end(), {0.0f, -gridSize, i});
        gridVertices.insert(gridVertices.end(), {0.0f, gridSize, i});
        
        gridVertices.insert(gridVertices.end(), {0.0f, i, -gridSize});
        gridVertices.insert(gridVertices.end(), {0.0f, i, gridSize});
    }

    // XY plane
    for (float i = -gridSize; i <= gridSize; i += gridSpacing) {
        gridVertices.insert(gridVertices.end(), {-gridSize, i, 0.0f});
        gridVertices.insert(gridVertices.end(), {gridSize, i, 0.0f});
        
        gridVertices.insert(gridVertices.end(), {i, -gridSize, 0.0f});
        gridVertices.insert(gridVertices.end(), {i, gridSize, 0.0f});
    }

    gridVertexCount_ = static_cast<int>(gridVertices.size() / 3);

    glGenVertexArrays(1, &VAO_grid_);
    glGenBuffers(1, &VBO_grid_);

    glBindVertexArray(VAO_grid_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_grid_);
    glBufferData(GL_ARRAY_BUFFER, 
                gridVertices.size() * sizeof(float),
                gridVertices.data(),
                GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GridRenderer::setupAxesBuffers() {
    constexpr float axesLength = 1000.0f;
    const float axes[] = {
        -axesLength,  0.0f, 0.0f,  // X axis start
         axesLength,  0.0f, 0.0f,  // X axis end
        
         0.0f, -axesLength, 0.0f,  // Y axis start
         0.0f,  axesLength, 0.0f,  // Y axis end
        
         0.0f, 0.0f, -axesLength,  // Z axis start
         0.0f, 0.0f,  axesLength   // Z axis end
    };

    glGenVertexArrays(1, &VAO_axes_);
    glGenBuffers(1, &VBO_axes_);

    glBindVertexArray(VAO_axes_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_axes_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GridRenderer::cleanupBuffers() {
    if (VBO_axes_ != 0) {
        glDeleteBuffers(1, &VBO_axes_);
        VBO_axes_ = 0;
    }
    if (VAO_axes_ != 0) {
        glDeleteVertexArrays(1, &VAO_axes_);
        VAO_axes_ = 0;
    }
    if (VBO_grid_ != 0) {
        glDeleteBuffers(1, &VBO_grid_);
        VBO_grid_ = 0;
    }
    if (VAO_grid_ != 0) {
        glDeleteVertexArrays(1, &VAO_grid_);
        VAO_grid_ = 0;
    }
}

} // namespace graphgl

