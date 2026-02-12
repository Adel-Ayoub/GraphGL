#pragma once

#include "equation.h"
#include "equation_parser.h"
#include <vector>
#include <functional>
#include <algorithm>

namespace graphgl {

/// Produces vertex data for equations using adaptive subdivision sampling.
class EquationGenerator {
public:
    EquationGenerator();
    ~EquationGenerator() = default;

    /// Populate equation.vertices (and indices for meshes) from the parsed expression.
    void generateVertices(Equation& equation, EquationParser& parser, 
                          int maxDepth = 6, double derivativeThreshold = 5.0);

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

