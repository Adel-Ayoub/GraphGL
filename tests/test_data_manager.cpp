#include <gtest/gtest.h>
#include "data_manager.h"
#include <cstdio>
#include <filesystem>

using namespace graphgl;

class DataManagerTest : public ::testing::Test {
protected:
    DataManager dm;
    std::string tmpFile;

    void SetUp() override {
        tmpFile = std::filesystem::temp_directory_path().string() + "/graphgl_test.mat";
    }

    void TearDown() override {
        std::remove(tmpFile.c_str());
    }
};

TEST_F(DataManagerTest, ExportCreatesFile) {
    std::vector<Equation> eqs;
    std::vector<Point> pts;
    EXPECT_TRUE(dm.exportData(tmpFile, eqs, pts));
    EXPECT_TRUE(std::filesystem::exists(tmpFile));
}

TEST_F(DataManagerTest, RoundtripEquation) {
    Equation eq;
    eq.expression = "sin(x)";
    eq.color = {0.5f, 0.6f, 0.7f};
    eq.sampleSize = 500;
    eq.minX = -10.0f;
    eq.maxX = 10.0f;
    eq.minY = -5.0f;
    eq.maxY = 5.0f;
    eq.isVisible = true;
    eq.is3D = false;

    std::vector<Equation> exported = {eq};
    std::vector<Point> noPts;

    ASSERT_TRUE(dm.exportData(tmpFile, exported, noPts));

    std::vector<Equation> imported;
    std::vector<Point> importedPts;
    ASSERT_TRUE(dm.importData(tmpFile, imported, importedPts));

    ASSERT_EQ(imported.size(), 1u);
    const auto& got = imported[0];
    EXPECT_EQ(got.expression, "sin(x)");
    EXPECT_NEAR(got.color[0], 0.5f, 1e-3f);
    EXPECT_NEAR(got.color[1], 0.6f, 1e-3f);
    EXPECT_NEAR(got.color[2], 0.7f, 1e-3f);
    EXPECT_EQ(got.sampleSize, 500);
    EXPECT_NEAR(got.minX, -10.0f, 1e-3f);
    EXPECT_NEAR(got.maxX, 10.0f, 1e-3f);
    EXPECT_TRUE(got.isVisible);
    EXPECT_FALSE(got.is3D);
}

TEST_F(DataManagerTest, RoundtripPoint) {
    Point pt;
    pt.position = glm::vec3(1.0f, 2.0f, 3.0f);
    pt.color = {0.1f, 0.2f, 0.3f};

    std::vector<Equation> noEqs;
    std::vector<Point> exported = {pt};

    ASSERT_TRUE(dm.exportData(tmpFile, noEqs, exported));

    std::vector<Equation> importedEqs;
    std::vector<Point> imported;
    ASSERT_TRUE(dm.importData(tmpFile, importedEqs, imported));

    ASSERT_EQ(imported.size(), 1u);
    EXPECT_NEAR(imported[0].position.x, 1.0f, 1e-3f);
    EXPECT_NEAR(imported[0].position.y, 2.0f, 1e-3f);
    EXPECT_NEAR(imported[0].position.z, 3.0f, 1e-3f);
    EXPECT_NEAR(imported[0].color[0], 0.1f, 1e-3f);
}

TEST_F(DataManagerTest, RoundtripMultipleEquationsAndPoints) {
    Equation eq1;
    eq1.expression = "x^2";
    eq1.is3D = true;
    eq1.isVisible = true;

    Equation eq2;
    eq2.expression = "cos(x) + y";
    eq2.is3D = true;
    eq2.isVisible = false;

    Point pt;
    pt.position = glm::vec3(5.0f, 6.0f, 7.0f);

    std::vector<Equation> eqs = {eq1, eq2};
    std::vector<Point> pts = {pt};
    ASSERT_TRUE(dm.exportData(tmpFile, eqs, pts));

    std::vector<Equation> iEqs;
    std::vector<Point> iPts;
    ASSERT_TRUE(dm.importData(tmpFile, iEqs, iPts));

    ASSERT_EQ(iEqs.size(), 2u);
    EXPECT_EQ(iEqs[0].expression, "x^2");
    EXPECT_EQ(iEqs[1].expression, "cos(x) + y");
    EXPECT_FALSE(iEqs[1].isVisible);
    ASSERT_EQ(iPts.size(), 1u);
}

TEST_F(DataManagerTest, ImportNonexistentFileFails) {
    std::vector<Equation> eqs;
    std::vector<Point> pts;
    EXPECT_FALSE(dm.importData("/tmp/no_such_file_graphgl.mat", eqs, pts));
}

TEST_F(DataManagerTest, ExportAddsMatExtension) {
    std::string noExt = std::filesystem::temp_directory_path().string() + "/graphgl_noext";
    std::vector<Equation> eqs;
    std::vector<Point> pts;
    ASSERT_TRUE(dm.exportData(noExt, eqs, pts));
    EXPECT_TRUE(std::filesystem::exists(noExt + ".mat"));
    std::remove((noExt + ".mat").c_str());
}
