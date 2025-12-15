#include "equation_parser.h"
#include "../lib/exprtk/exprtk.hpp"
#include <iostream>
#include <cmath>

namespace graphgl {

class EquationParser::Impl {
public:
    exprtk::symbol_table<float> symbolTable;
    exprtk::expression<float> expression;
    exprtk::parser<float> parser;
    
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    bool is3D = true;
    
    void setupSymbolTable() {
        constexpr double e = 2.71828182845904523536028747135266249775724709369996;
        
        symbolTable.add_constant("e", e);
        symbolTable.add_pi();
        symbolTable.add_variable("x", x);
        symbolTable.add_variable("y", y);
        symbolTable.add_variable("z", z);
        
        expression.register_symbol_table(symbolTable);
    }
};

EquationParser::EquationParser()
    : pImpl_(std::make_unique<Impl>())
    , isValid_(false)
    , errorMessage_("")
{
    pImpl_->setupSymbolTable();
}

EquationParser::~EquationParser() = default;

bool EquationParser::parseExpression(const std::string& expr, bool is3D) {
    isValid_ = false;
    errorMessage_.clear();
    
    if (expr.empty()) {
        errorMessage_ = "Expression is empty";
        return false;
    }
    
    pImpl_->is3D = is3D;
    
    // Compile the expression
    if (!pImpl_->parser.compile(expr, pImpl_->expression)) {
        errorMessage_ = "Failed to compile expression: " + expr;
        return false;
    }
    
    isValid_ = true;
    return true;
}

float EquationParser::evaluate(float x, float y) const {
    if (!isValid_) {
        return std::nanf("");
    }
    
    try {
        pImpl_->x = x;
        pImpl_->y = pImpl_->is3D ? y : 0.0f;
        pImpl_->z = 0.0f;
        
        return pImpl_->expression.value();
    }
    catch (...) {
        return std::nanf("");
    }
}

} // namespace graphgl

