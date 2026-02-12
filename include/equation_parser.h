#pragma once

#include "equation.h"
#include <string>
#include <memory>

namespace graphgl {

/// Wraps ExprTk behind a PIMPL to parse and evaluate math expressions.
class EquationParser {
public:
    EquationParser();
    ~EquationParser();

    EquationParser(const EquationParser&) = delete;
    EquationParser& operator=(const EquationParser&) = delete;

    /// Compile an expression string. Returns false on syntax errors.
    [[nodiscard]] bool parseExpression(const std::string& expression, bool is3D = true);

    /// Evaluate the last compiled expression at (x, y).
    float evaluate(float x, float y = 0.0f) const;

    bool isValid() const { return isValid_; }
    std::string getErrorMessage() const { return errorMessage_; }

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    bool isValid_;
    std::string errorMessage_;

};

} // namespace graphgl

