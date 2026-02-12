#pragma once

#include "equation.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace graphgl {

/// Manages OpenGL buffers and draw calls for equation curves, surfaces, and points.
class EquationRenderer {
public:
    EquationRenderer();
    ~EquationRenderer();

    EquationRenderer(const EquationRenderer&) = delete;
    EquationRenderer& operator=(const EquationRenderer&) = delete;

    void initialize();

    /// Re-upload vertex/index data from the current equations and points.
    void updateVertices(const std::vector<Equation>& equations, const std::vector<Point>& points);

    /// Issue draw calls for meshes (indexed) and standalone points.
    void render(const Shader& shader, bool useHeatmap, float minHeight, float maxHeight) const;

    size_t getVertexCount() const { return vertices_.size(); }
    size_t getIndexCount() const { return indices_.size(); }

private:
    unsigned int VAO_;
    unsigned int VBO_;
    unsigned int EBO_;
    
    std::vector<glm::vec3> vertices_;
    std::vector<unsigned int> indices_;
    size_t equationVertexCount_; // vec3 entries belonging to equations (before points).
    
    bool initialized_;

    void setupBuffers();
    void cleanupBuffers();
};

} // namespace graphgl

