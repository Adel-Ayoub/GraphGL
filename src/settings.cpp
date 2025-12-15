#include "settings.h"

namespace graphgl {

Settings::Settings()
    : width_(DEFAULT_WIDTH)
    , height_(DEFAULT_HEIGHT)
    , maxViewDistance_(DEFAULT_MAX_VIEW_DISTANCE)
    , pointSize_(DEFAULT_POINT_SIZE)
    , maxDepth_(DEFAULT_MAX_DEPTH)
    , derivativeThreshold_(DEFAULT_DERIVATIVE_THRESHOLD)
    , minX_(DEFAULT_MIN_X)
    , maxX_(DEFAULT_MAX_X)
    , minY_(DEFAULT_MIN_Y)
    , maxY_(DEFAULT_MAX_Y)
    , showControls_(DEFAULT_SHOW_CONTROLS)
    , useHeatmap_(DEFAULT_USE_HEATMAP)
    , showGridlines_(DEFAULT_SHOW_GRIDLINES)
    , showLines_(DEFAULT_SHOW_LINES)
    , minHeight_(FLT_MAX)
    , maxHeight_(-FLT_MAX)
{
}

void Settings::resetHeightTracking() {
    minHeight_ = FLT_MAX;
    maxHeight_ = -FLT_MAX;
}

} // namespace graphgl

