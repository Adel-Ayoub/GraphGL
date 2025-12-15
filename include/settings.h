#pragma once

#include <string>
#include <limits>
#include <cfloat>

namespace graphgl {

class Settings {
public:
    // Window settings
    static constexpr int DEFAULT_WIDTH = 1280;
    static constexpr int DEFAULT_HEIGHT = 720;
    
    // Rendering settings
    static constexpr float DEFAULT_MAX_VIEW_DISTANCE = 250.0f;
    static constexpr float DEFAULT_POINT_SIZE = 1.0f;
    static constexpr int DEFAULT_MAX_DEPTH = 6;
    static constexpr double DEFAULT_DERIVATIVE_THRESHOLD = 5.0;
    
    // Domain settings
    static constexpr float DEFAULT_MIN_X = -100.0f;
    static constexpr float DEFAULT_MAX_X = 100.0f;
    static constexpr float DEFAULT_MIN_Y = -100.0f;
    static constexpr float DEFAULT_MAX_Y = 100.0f;
    
    // UI settings
    static constexpr bool DEFAULT_SHOW_CONTROLS = true;
    static constexpr bool DEFAULT_USE_HEATMAP = false;
    static constexpr bool DEFAULT_SHOW_GRIDLINES = true;
    static constexpr bool DEFAULT_SHOW_LINES = true;

    Settings();
    ~Settings() = default;

    // Window dimensions
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    void setWidth(int width) { width_ = width; }
    void setHeight(int height) { height_ = height; }

    // Rendering settings
    float getMaxViewDistance() const { return maxViewDistance_; }
    void setMaxViewDistance(float distance) { maxViewDistance_ = distance; }
    
    float getPointSize() const { return pointSize_; }
    void setPointSize(float size) { pointSize_ = size; }
    
    int getMaxDepth() const { return maxDepth_; }
    void setMaxDepth(int depth) { maxDepth_ = depth; }
    
    double getDerivativeThreshold() const { return derivativeThreshold_; }
    void setDerivativeThreshold(double threshold) { derivativeThreshold_ = threshold; }

    // Domain settings
    float getMinX() const { return minX_; }
    float getMaxX() const { return maxX_; }
    float getMinY() const { return minY_; }
    float getMaxY() const { return maxY_; }
    
    void setMinX(float min) { minX_ = min; }
    void setMaxX(float max) { maxX_ = max; }
    void setMinY(float min) { minY_ = min; }
    void setMaxY(float max) { maxY_ = max; }

    // UI settings
    bool getShowControls() const { return showControls_; }
    void setShowControls(bool show) { showControls_ = show; }
    
    bool getUseHeatmap() const { return useHeatmap_; }
    void setUseHeatmap(bool use) { useHeatmap_ = use; }
    
    bool getShowGridlines() const { return showGridlines_; }
    void setShowGridlines(bool show) { showGridlines_ = show; }
    
    bool getShowLines() const { return showLines_; }
    void setShowLines(bool show) { showLines_ = show; }

    // Height tracking
    float getMinHeight() const { return minHeight_; }
    float getMaxHeight() const { return maxHeight_; }
    void setMinHeight(float min) { minHeight_ = min; }
    void setMaxHeight(float max) { maxHeight_ = max; }
    void resetHeightTracking();

private:
    int width_;
    int height_;
    
    float maxViewDistance_;
    float pointSize_;
    int maxDepth_;
    double derivativeThreshold_;
    
    float minX_;
    float maxX_;
    float minY_;
    float maxY_;
    
    bool showControls_;
    bool useHeatmap_;
    bool showGridlines_;
    bool showLines_;
    
    float minHeight_;
    float maxHeight_;
};

} // namespace graphgl

