#include "application.h"
#include <iostream>
#include <string>
#include <cstring>

static void printUsage(const char* program) {
    std::cout << "Usage: " << program << " [options]\n"
              << "  --width  <int>     Window width  (default: 1280)\n"
              << "  --height <int>     Window height (default: 720)\n"
              << "  --title  <string>  Window title  (default: GraphGL)\n"
              << "  --file   <path>    Auto-import a .mat file on startup\n"
              << "  --help             Show this message\n";
}

int main(int argc, char* argv[]) {
    int width = 1280;
    int height = 720;
    std::string title = "GraphGL";
    std::string importFile;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (std::strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            width = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            height = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--title") == 0 && i + 1 < argc) {
            title = argv[++i];
        } else if (std::strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            importFile = argv[++i];
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    graphgl::Application app;

    if (!app.initialize(width, height, title.c_str())) {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }

    if (!importFile.empty()) {
        app.importFile(importFile);
    }

    app.run();

    return 0;
}

