#pragma once

#include "equation.h"
#include <string>
#include <memory>

namespace graphgl {

class EquationParser {
public:
    EquationParser();
    ~EquationParser();

    // Delete copy constructor and assignment
    EquationParser(const EquationParser&) = delete;
    EquationParser& operator=(const EquationParser&) = delete;

    // Parse and validate expression
    bool parseExpression(const std::string& expression, bool is3D = true);

    // Evaluate expression at given point
    float evaluate(float x, float y = 0.0f) const;

    // Check if parser is ready
    bool isValid() const { return isValid_; }

    // Get error message if parsing failed
    std::string getErrorMessage() const { return errorMessage_; }

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    bool isValid_;
    std::string errorMessage_;

    void setupSymbolTable();
};

} // namespace graphgl

