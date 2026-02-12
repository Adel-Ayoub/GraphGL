#include <gtest/gtest.h>
#include "equation_parser.h"
#include <cmath>

using namespace graphgl;

class EquationParserTest : public ::testing::Test {
protected:
    EquationParser parser;
};

TEST_F(EquationParserTest, EmptyExpression) {
    EXPECT_FALSE(parser.parseExpression(""));
    EXPECT_FALSE(parser.isValid());
}

TEST_F(EquationParserTest, InvalidSyntax) {
    EXPECT_FALSE(parser.parseExpression("+++"));
    EXPECT_FALSE(parser.isValid());
}

TEST_F(EquationParserTest, SimpleConstant) {
    ASSERT_TRUE(parser.parseExpression("42"));
    EXPECT_FLOAT_EQ(parser.evaluate(0.0f), 42.0f);
}

TEST_F(EquationParserTest, LinearExpression) {
    ASSERT_TRUE(parser.parseExpression("2*x + 1"));
    EXPECT_FLOAT_EQ(parser.evaluate(0.0f), 1.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(3.0f), 7.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(-1.0f), -1.0f);
}

TEST_F(EquationParserTest, Quadratic) {
    ASSERT_TRUE(parser.parseExpression("x^2"));
    EXPECT_FLOAT_EQ(parser.evaluate(0.0f), 0.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(3.0f), 9.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(-4.0f), 16.0f);
}

TEST_F(EquationParserTest, SinFunction) {
    ASSERT_TRUE(parser.parseExpression("sin(x)"));
    EXPECT_NEAR(parser.evaluate(0.0f), 0.0f, 1e-6f);
    EXPECT_NEAR(parser.evaluate(M_PI / 2.0f), 1.0f, 1e-5f);
}

TEST_F(EquationParserTest, TwoVariable3D) {
    ASSERT_TRUE(parser.parseExpression("x + y", true));
    EXPECT_FLOAT_EQ(parser.evaluate(2.0f, 3.0f), 5.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(-1.0f, 4.0f), 3.0f);
}

TEST_F(EquationParserTest, TwoVariable3DSurface) {
    ASSERT_TRUE(parser.parseExpression("x^2 + y^2", true));
    EXPECT_FLOAT_EQ(parser.evaluate(0.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(1.0f, 1.0f), 2.0f);
    EXPECT_FLOAT_EQ(parser.evaluate(3.0f, 4.0f), 25.0f);
}

TEST_F(EquationParserTest, BuiltInConstants) {
    ASSERT_TRUE(parser.parseExpression("pi"));
    EXPECT_NEAR(parser.evaluate(0.0f), M_PI, 1e-5f);

    ASSERT_TRUE(parser.parseExpression("e"));
    EXPECT_NEAR(parser.evaluate(0.0f), M_E, 1e-5f);
}

TEST_F(EquationParserTest, EvaluateBeforeParseFails) {
    // No expression compiled yet.
    float result = parser.evaluate(1.0f);
    EXPECT_TRUE(std::isnan(result));
}

TEST_F(EquationParserTest, RecompileOverwritesPrevious) {
    ASSERT_TRUE(parser.parseExpression("x"));
    EXPECT_FLOAT_EQ(parser.evaluate(5.0f), 5.0f);

    ASSERT_TRUE(parser.parseExpression("x^2"));
    EXPECT_FLOAT_EQ(parser.evaluate(5.0f), 25.0f);
}
