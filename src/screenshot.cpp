#include "screenshot.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace graphgl {

bool saveScreenshot(const std::string& filename, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // OpenGL reads bottom-to-top; flip vertically for a normal image.
    std::vector<unsigned char> flipped(pixels.size());
    int rowBytes = width * 3;
    for (int y = 0; y < height; ++y) {
        std::copy(pixels.begin() + (height - 1 - y) * rowBytes,
                  pixels.begin() + (height - y) * rowBytes,
                  flipped.begin() + y * rowBytes);
    }

    std::string path = filename;
    if (path.find(".png") == std::string::npos) {
        path += ".png";
    }

    if (!stbi_write_png(path.c_str(), width, height, 3, flipped.data(), rowBytes)) {
        std::cerr << "Failed to write screenshot: " << path << std::endl;
        return false;
    }

    std::cout << "Screenshot saved: " << path << std::endl;
    return true;
}

} // namespace graphgl
