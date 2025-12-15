#pragma once

#include "shader.h"
#include <glm/glm.hpp>

namespace graphgl {

class GridRenderer {
public:
    GridRenderer();
    ~GridRenderer();

    // Delete copy constructor and assignment
    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    // Initialize grid renderer
    void initialize(float gridSize = 1000.0f, float gridSpacing = 1.0f);

    // Render grid lines
    void renderGridLines(const Shader& shader) const;

    // Render axes
    void renderAxes(const Shader& shader) const;

    // Set visibility
    void setGridLinesVisible(bool visible) { gridLinesVisible_ = visible; }
    void setAxesVisible(bool visible) { axesVisible_ = visible; }

    bool getGridLinesVisible() const { return gridLinesVisible_; }
    bool getAxesVisible() const { return axesVisible_; }

private:
    unsigned int VAO_grid_;
    unsigned int VBO_grid_;
    unsigned int VAO_axes_;
    unsigned int VBO_axes_;
    
    int gridVertexCount_;
    int axesVertexCount_;
    
    bool gridLinesVisible_;
    bool axesVisible_;
    bool initialized_;

    void setupGridBuffers(float gridSize, float gridSpacing);
    void setupAxesBuffers();
    void cleanupBuffers();
};

} // namespace graphgl

