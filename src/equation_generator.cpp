#include "equation_generator.h"
#include <cmath>
#include <limits>

namespace graphgl {

EquationGenerator::EquationGenerator()
    : minHeight_(std::numeric_limits<float>::max())
    , maxHeight_(-std::numeric_limits<float>::max())
{
}

void EquationGenerator::generateVertices(Equation& equation, EquationParser& parser,
                                        int maxDepth, double derivativeThreshold) {
    equation.vertices.clear();
    equation.indices.clear();
    minHeight_ = std::numeric_limits<float>::max();
    maxHeight_ = -std::numeric_limits<float>::max();

    if (equation.is3D) {
        // Generate 3D surface
        auto xSamples = adaptiveSample(
            [&](float x) { return safeEvaluate(parser, x, equation.minY, equation.is3D); },
            equation.minX,
            equation.maxX,
            maxDepth,
            derivativeThreshold
        );

        auto ySamples = adaptiveSample(
            [&](float y) { return safeEvaluate(parser, equation.minX, y, equation.is3D); },
            equation.minY,
            equation.maxY,
            maxDepth,
            derivativeThreshold
        );

        // Generate vertices for 3D surface
        for (float x : xSamples) {
            for (float y : ySamples) {
                float z = safeEvaluate(parser, x, y, equation.is3D);
                if (!std::isnan(z)) {
                    equation.vertices.emplace_back(x, z, y);
                    equation.vertices.emplace_back(glm::vec3(equation.color[0], equation.color[1], equation.color[2]));
                    minHeight_ = std::min(minHeight_, z);
                    maxHeight_ = std::max(maxHeight_, z);
                }
            }
        }

        // Generate mesh indices if requested
        if (equation.isMesh) {
            const size_t cols = xSamples.size();
            const size_t rows = ySamples.size();

            for (size_t y = 0; y < rows - 1; ++y) {
                for (size_t x = 0; x < cols - 1; ++x) {
                    const size_t i0 = y * cols + x;
                    const size_t i1 = y * cols + (x + 1);
                    const size_t i2 = (y + 1) * cols + x;
                    const size_t i3 = (y + 1) * cols + (x + 1);

                    // Check if vertices are valid (not NaN)
                    if (i0 * 2 < equation.vertices.size() &&
                        i1 * 2 < equation.vertices.size() &&
                        i2 * 2 < equation.vertices.size() &&
                        i3 * 2 < equation.vertices.size()) {
                        
                        const float y0 = equation.vertices[i0 * 2].y;
                        const float y1 = equation.vertices[i1 * 2].y;
                        const float y2 = equation.vertices[i2 * 2].y;
                        const float y3 = equation.vertices[i3 * 2].y;

                        if (!std::isnan(y0) && !std::isnan(y1) && 
                            !std::isnan(y2) && !std::isnan(y3)) {
                            equation.indices.insert(equation.indices.end(), {
                                static_cast<unsigned int>(i0),
                                static_cast<unsigned int>(i1),
                                static_cast<unsigned int>(i2),
                                static_cast<unsigned int>(i1),
                                static_cast<unsigned int>(i3),
                                static_cast<unsigned int>(i2)
                            });
                        }
                    }
                }
            }
        }
    } else {
        // Generate 2D curve
        auto xSamples = adaptiveSample(
            [&](float x) { return safeEvaluate(parser, x, 0.0f, equation.is3D); },
            equation.minX,
            equation.maxX,
            maxDepth,
            derivativeThreshold
        );

        for (float x : xSamples) {
            float y = safeEvaluate(parser, x, 0.0f, equation.is3D);
            if (!std::isnan(y)) {
                equation.vertices.emplace_back(x, y, 0.0f);
                equation.vertices.emplace_back(glm::vec3(equation.color[0], equation.color[1], equation.color[2]));
                minHeight_ = std::min(minHeight_, y);
                maxHeight_ = std::max(maxHeight_, y);
            }
        }
    }
}

std::vector<float> EquationGenerator::adaptiveSample(
    const std::function<float(float)>& func,
    float min,
    float max,
    int maxDepth,
    double derivativeThreshold
) const {
    std::vector<float> samples;
    constexpr double epsilon = 1e-6;

    std::function<void(float, float, float, float, int)> subdivide = 
        [&](float x0, float x1, float y0, float y1, int depth) {
            if (depth >= maxDepth) {
                samples.push_back(x0);
                return;
            }

            const float xMid = (x0 + x1) * 0.5f;
            const float yMid = func(xMid);

            const float dyLeft = std::abs((yMid - y0) / (xMid - x0 + epsilon));
            const float dyRight = std::abs((y1 - yMid) / (x1 - xMid + epsilon));

            if (dyLeft > derivativeThreshold || dyRight > derivativeThreshold) {
                subdivide(x0, xMid, y0, yMid, depth + 1);
                subdivide(xMid, x1, yMid, y1, depth + 1);
            } else {
                samples.push_back(x0);
            }
        };

    // Create base samples
    const int baseSampleCount = 100;
    std::vector<float> baseX;
    for (int i = 0; i < baseSampleCount; ++i) {
        baseX.push_back(min + (max - min) * (i / float(baseSampleCount - 1)));
    }

    // Apply adaptive sampling
    for (size_t i = 0; i < baseX.size() - 1; ++i) {
        const float x0 = baseX[i];
        const float x1 = baseX[i + 1];
        subdivide(x0, x1, func(x0), func(x1), 0);
    }

    samples.push_back(max);
    std::sort(samples.begin(), samples.end());
    return samples;
}

float EquationGenerator::safeEvaluate(EquationParser& parser, float x, float y, bool /* is3D */) const {
    try {
        return parser.evaluate(x, y);
    }
    catch (...) {
        return std::nanf("");
    }
}

} // namespace graphgl

