#pragma once

#include <string>

namespace graphgl {

/// Capture the current OpenGL framebuffer and save it as a PNG file.
/// Must be called while an OpenGL context is current.
bool saveScreenshot(const std::string& filename, int width, int height);

} // namespace graphgl
