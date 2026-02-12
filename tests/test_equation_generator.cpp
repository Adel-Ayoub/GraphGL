#include <gtest/gtest.h>
#include "equation_generator.h"
#include "equation_parser.h"
#include <cmath>

using namespace graphgl;

class EquationGeneratorTest : public ::testing::Test {
protected:
    EquationGenerator gen;
    EquationParser parser;
};

TEST_F(EquationGeneratorTest, Generates2DVertices) {
    Equation eq;
    eq.expression = "x";
    eq.is3D = false;
    eq.minX = -1.0f;
    eq.maxX = 1.0f;

    ASSERT_TRUE(parser.parseExpression(eq.expression, eq.is3D));
    gen.generateVertices(eq, parser);

    // Should have produced some vertices (pairs of pos + color).
    EXPECT_GT(eq.vertices.size(), 0u);
    // Each vertex is 2 vec3 entries, so size must be even.
    EXPECT_EQ(eq.vertices.size() % 2, 0u);
}

TEST_F(EquationGeneratorTest, Generates3DVertices) {
    Equation eq;
    eq.expression = "x + y";
    eq.is3D = true;
    eq.minX = -1.0f;
    eq.maxX = 1.0f;
    eq.minY = -1.0f;
    eq.maxY = 1.0f;

    ASSERT_TRUE(parser.parseExpression(eq.expression, eq.is3D));
    gen.generateVertices(eq, parser);

    EXPECT_GT(eq.vertices.size(), 0u);
    EXPECT_EQ(eq.vertices.size() % 2, 0u);
}

TEST_F(EquationGeneratorTest, HeightTrackingUpdated) {
    Equation eq;
    eq.expression = "x^2";
    eq.is3D = false;
    eq.minX = -2.0f;
    eq.maxX = 2.0f;

    ASSERT_TRUE(parser.parseExpression(eq.expression, eq.is3D));
    gen.generateVertices(eq, parser);

    // x^2 on [-2,2]: min is 0, max is 4.
    EXPECT_NEAR(gen.getMinHeight(), 0.0f, 0.1f);
    EXPECT_NEAR(gen.getMaxHeight(), 4.0f, 0.1f);
}

TEST_F(EquationGeneratorTest, MeshGeneratesIndices) {
    Equation eq;
    eq.expression = "x + y";
    eq.is3D = true;
    eq.isMesh = true;
    eq.minX = -1.0f;
    eq.maxX = 1.0f;
    eq.minY = -1.0f;
    eq.maxY = 1.0f;

    ASSERT_TRUE(parser.parseExpression(eq.expression, eq.is3D));
    gen.generateVertices(eq, parser);

    EXPECT_GT(eq.indices.size(), 0u);
    // Triangle indices come in multiples of 6 (two triangles per quad).
    EXPECT_EQ(eq.indices.size() % 6, 0u);
}

TEST_F(EquationGeneratorTest, NonMeshHasNoIndices) {
    Equation eq;
    eq.expression = "x + y";
    eq.is3D = true;
    eq.isMesh = false;
    eq.minX = -1.0f;
    eq.maxX = 1.0f;
    eq.minY = -1.0f;
    eq.maxY = 1.0f;

    ASSERT_TRUE(parser.parseExpression(eq.expression, eq.is3D));
    gen.generateVertices(eq, parser);

    EXPECT_EQ(eq.indices.size(), 0u);
}
