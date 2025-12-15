#pragma once

#include "equation.h"
#include "equation_parser.h"
#include <vector>
#include <functional>
#include <algorithm>

namespace graphgl {

class EquationGenerator {
public:
    EquationGenerator();
    ~EquationGenerator() = default;

    // Generate vertices for an equation
    void generateVertices(Equation& equation, EquationParser& parser, 
                          int maxDepth = 6, double derivativeThreshold = 5.0);

    // Get min/max height values from last generation
    float getMinHeight() const { return minHeight_; }
    float getMaxHeight() const { return maxHeight_; }

private:
    float minHeight_;
    float maxHeight_;

    // Adaptive sampling helper
    std::vector<float> adaptiveSample(
        const std::function<float(float)>& func,
        float min,
        float max,
        int maxDepth,
        double derivativeThreshold
    ) const;

    // Safe evaluation wrapper
    float safeEvaluate(EquationParser& parser, float x, float y, bool is3D) const;
};

} // namespace graphgl

