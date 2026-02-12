#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <array>

namespace graphgl {

struct Equation {
    std::string expression;
    std::array<float, 3> color = {1.0f, 0.5f, 0.2f};
    int sampleSize = 1000;
    float minX = -25.0f;
    float maxX = 25.0f;
    float minY = -25.0f;
    float maxY = 25.0f;
    std::vector<glm::vec3> vertices;
    bool is3D = true;
    bool isVisible = true;
    float opacity = 1.0f;
    bool isMesh = false;
    std::vector<unsigned int> indices;
};

struct Point {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    std::array<float, 3> color = {1.0f, 0.5f, 0.2f};
    std::vector<glm::vec3> vertexData;
};

} // namespace graphgl

