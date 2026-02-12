#include <gtest/gtest.h>
#include "settings.h"
#include <cfloat>

using namespace graphgl;

TEST(SettingsTest, DefaultValues) {
    Settings s;
    EXPECT_EQ(s.getWidth(), Settings::DEFAULT_WIDTH);
    EXPECT_EQ(s.getHeight(), Settings::DEFAULT_HEIGHT);
    EXPECT_FLOAT_EQ(s.getMaxViewDistance(), Settings::DEFAULT_MAX_VIEW_DISTANCE);
    EXPECT_FLOAT_EQ(s.getPointSize(), Settings::DEFAULT_POINT_SIZE);
    EXPECT_EQ(s.getMaxDepth(), Settings::DEFAULT_MAX_DEPTH);
    EXPECT_DOUBLE_EQ(s.getDerivativeThreshold(), Settings::DEFAULT_DERIVATIVE_THRESHOLD);
    EXPECT_TRUE(s.getShowControls());
    EXPECT_FALSE(s.getUseHeatmap());
    EXPECT_TRUE(s.getShowGridlines());
    EXPECT_TRUE(s.getShowLines());
}

TEST(SettingsTest, SettersAndGetters) {
    Settings s;
    s.setWidth(800);
    s.setHeight(600);
    EXPECT_EQ(s.getWidth(), 800);
    EXPECT_EQ(s.getHeight(), 600);

    s.setMaxViewDistance(500.0f);
    EXPECT_FLOAT_EQ(s.getMaxViewDistance(), 500.0f);

    s.setPointSize(3.0f);
    EXPECT_FLOAT_EQ(s.getPointSize(), 3.0f);

    s.setUseHeatmap(true);
    EXPECT_TRUE(s.getUseHeatmap());
}

TEST(SettingsTest, HeightTracking) {
    Settings s;
    // Initial state should be FLT_MAX / -FLT_MAX (no data yet).
    EXPECT_FLOAT_EQ(s.getMinHeight(), FLT_MAX);
    EXPECT_FLOAT_EQ(s.getMaxHeight(), -FLT_MAX);

    s.setMinHeight(-5.0f);
    s.setMaxHeight(10.0f);
    EXPECT_FLOAT_EQ(s.getMinHeight(), -5.0f);
    EXPECT_FLOAT_EQ(s.getMaxHeight(), 10.0f);

    s.resetHeightTracking();
    EXPECT_FLOAT_EQ(s.getMinHeight(), FLT_MAX);
    EXPECT_FLOAT_EQ(s.getMaxHeight(), -FLT_MAX);
}

TEST(SettingsTest, DomainSettings) {
    Settings s;
    s.setMinX(-50.0f);
    s.setMaxX(50.0f);
    s.setMinY(-30.0f);
    s.setMaxY(30.0f);
    EXPECT_FLOAT_EQ(s.getMinX(), -50.0f);
    EXPECT_FLOAT_EQ(s.getMaxX(), 50.0f);
    EXPECT_FLOAT_EQ(s.getMinY(), -30.0f);
    EXPECT_FLOAT_EQ(s.getMaxY(), 30.0f);
}
