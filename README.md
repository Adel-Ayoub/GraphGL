# GraphGL

An interactive 2D/3D equation graphing application built with C++17 and OpenGL.

## Features

- Parse and render arbitrary mathematical expressions (e.g. `sin(x)`, `x^2 + y^2`)
- Adaptive sampling for accurate curve representation
- 3D surface rendering with mesh mode
- Heatmap coloring based on height
- Interactive camera (orbit, pan, zoom, roll)
- Import/export equations and points (`.mat` format)
- ImGui-powered control panel

## Prerequisites

| Dependency | Notes |
|------------|-------|
| C++17 compiler | GCC, Clang, or MSVC |
| GLFW 3 | Window management and input |
| GLM | Vector/matrix math |
| OpenGL 3.3+ | Core profile |

**macOS (Homebrew):**

```bash
brew install glfw glm
```

**Ubuntu/Debian:**

```bash
sudo apt install libglfw3-dev libglm-dev
```

Bundled libraries (git submodules): ImGui, ExprTk, stb, GLAD.

## Building

```bash
git clone --recursive https://github.com/your-username/GraphGL.git
cd GraphGL
make                        # debug build
make BUILD_MODE=release     # release build
```

## Running

```bash
make run
# or
./build/graphgl
```

## Controls

| Key | Action |
|-----|--------|
| W/A/S/D | Move camera |
| Space / C | Move up |
| Ctrl / X | Move down |
| Q / E | Roll left / right |
| I | Reset camera to origin |
| ` / TAB / M | Toggle mouse look |
| H | Toggle heatmap |
| Escape | Quit |

## Import/Export Format

The `.mat` file format stores equations and points as plain text:

```
Equation "r g b sampleSize minX maxX minY maxY visible is3D expression"
Point px py pz r g b
```

## Architecture

```
src/
├── main.cpp               Entry point
├── application.cpp         Main loop, input, subsystem wiring
├── camera.cpp              3D camera (orbit, pan, zoom)
├── renderer.cpp            Base OpenGL renderer
├── equation_renderer.cpp   Equation/point draw calls
├── grid_renderer.cpp       Grid and axis overlay
├── equation_parser.cpp     Expression parsing (ExprTk, PIMPL)
├── equation_generator.cpp  Adaptive sampling and vertex generation
├── data_manager.cpp        Import/export .mat files
├── ui_controller.cpp       ImGui panels and callbacks
├── settings.cpp            Rendering and UI options
└── shader.cpp              Shader loading and uniform management
```

## License

See [LICENSE](LICENSE) for details.
