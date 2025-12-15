#pragma once

#include "equation.h"
#include "settings.h"
#include "shader.h"
#include "camera.h"
#include "data_manager.h"
#include "equation_parser.h"
#include "equation_generator.h"
#include <vector>
#include <string>
#include <functional>

// Forward declarations
struct GLFWwindow;

namespace graphgl {

class UIController {
public:
    UIController();
    ~UIController();

    // Delete copy constructor and assignment
    UIController(const UIController&) = delete;
    UIController& operator=(const UIController&) = delete;

    // Initialize ImGui
    bool initialize(GLFWwindow* window);

    // Shutdown ImGui
    void shutdown();

    // Render UI frame
    void renderFrame();

    // Set callbacks for user actions
    void setOnEquationRender(std::function<void(Equation&, size_t)> callback);
    void setOnEquationRemove(std::function<void(size_t)> callback);
    void setOnPointRender(std::function<void(Point&, size_t)> callback);
    void setOnPointRemove(std::function<void(size_t)> callback);
    void setOnEquationAdd(std::function<void()> callback);
    void setOnPointAdd(std::function<void()> callback);
    void setOnImport(std::function<void(const std::string&)> callback);
    void setOnExport(std::function<void(const std::string&)> callback);

    // Set data references
    void setEquations(std::vector<Equation>* equations) { equations_ = equations; }
    void setPoints(std::vector<Point>* points) { points_ = points; }
    void setSettings(Settings* settings) { settings_ = settings; }
    void setCamera(Camera* camera) { camera_ = camera; }

    // Get UI state
    bool getMouseFocus() const { return mouseFocus_; }
    void setMouseFocus(bool focus) { mouseFocus_ = focus; }

private:
    GLFWwindow* window_;
    Settings* settings_;
    Camera* camera_;
    std::vector<Equation>* equations_;
    std::vector<Point>* points_;

    bool mouseFocus_;
    bool initialized_;
    int currentCursorMode_;  // Track current cursor mode to avoid redundant calls

    // Input buffers
    char importFilepath_[256];
    char exportFilepath_[256];

    // Callbacks
    std::function<void(Equation&, size_t)> onEquationRender_;
    std::function<void(size_t)> onEquationRemove_;
    std::function<void(Point&, size_t)> onPointRender_;
    std::function<void(size_t)> onPointRemove_;
    std::function<void()> onEquationAdd_;
    std::function<void()> onPointAdd_;
    std::function<void(const std::string&)> onImport_;
    std::function<void(const std::string&)> onExport_;

    // UI rendering methods
    void renderMainMenuBar();
    void renderControlsPopup();
    void renderEquationInput(Equation& equation, size_t index);
    void renderPointInput(Point& point, size_t index);
    void renderEquations();
    void renderPoints();
    void renderMainWindow();
};

} // namespace graphgl

