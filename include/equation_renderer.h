#pragma once

#include "equation.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace graphgl {

class EquationRenderer {
public:
    EquationRenderer();
    ~EquationRenderer();

    // Delete copy constructor and assignment
    EquationRenderer(const EquationRenderer&) = delete;
    EquationRenderer& operator=(const EquationRenderer&) = delete;

    // Initialize renderer
    void initialize();

    // Update vertex data for equations
    void updateVertices(const std::vector<Equation>& equations, const std::vector<Point>& points);

    // Render equations
    void render(const Shader& shader, bool useHeatmap, float minHeight, float maxHeight) const;

    // Get vertex count
    size_t getVertexCount() const { return vertices_.size(); }
    size_t getIndexCount() const { return indices_.size(); }

private:
    unsigned int VAO_;
    unsigned int VBO_;
    unsigned int EBO_;
    
    std::vector<glm::vec3> vertices_;
    std::vector<unsigned int> indices_;
    
    bool initialized_;

    void setupBuffers();
    void cleanupBuffers();
};

} // namespace graphgl

