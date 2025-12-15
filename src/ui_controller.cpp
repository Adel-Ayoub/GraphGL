#include "ui_controller.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_opengl3.h"
#include <glm/gtx/string_cast.hpp>
#include <cstring>

namespace graphgl {

UIController::UIController()
    : window_(nullptr)
    , settings_(nullptr)
    , camera_(nullptr)
    , equations_(nullptr)
    , points_(nullptr)
    , mouseFocus_(false)
    , initialized_(false)
    , currentCursorMode_(-1)  // Initialize to invalid value to force first update
{
    importFilepath_[0] = '\0';
    exportFilepath_[0] = '\0';
}

UIController::~UIController() {
    shutdown();
}

bool UIController::initialize(GLFWwindow* window) {
    if (initialized_) {
        return true;
    }

    window_ = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // Initialize cursor to visible state
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    currentCursorMode_ = GLFW_CURSOR_NORMAL;
    
    initialized_ = true;
    return true;
}

void UIController::shutdown() {
    if (!initialized_) {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    initialized_ = false;
}

void UIController::renderFrame() {
    // This method is deprecated - ImGui setup is now done in Application::render()
    // Keeping for compatibility but it should not be called
    renderMainWindow();
    renderControlsPopup();
}

void UIController::renderMainWindow() {
    if (!settings_ || !equations_ || !points_ || !camera_) {
        return;
    }

    ImGui::Begin("GraphGL");

    renderMainMenuBar();
    renderEquations();
    renderPoints();

    if (ImGui::Button("Add Equation")) {
        if (onEquationAdd_) {
            onEquationAdd_();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Point")) {
        if (onPointAdd_) {
            onPointAdd_();
        }
    }

    // Display stats
    ImGui::Text("Camera Position: %s", glm::to_string(camera_->getPosition()).c_str());
    
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("%.1f FPS", io.Framerate);
    
    ImGui::Text("Min Height: %.2f", settings_->getMinHeight());
    ImGui::Text("Max Height: %.2f", settings_->getMaxHeight());

    ImGui::End();
}

void UIController::renderMainMenuBar() {
    if (!settings_) {
        return;
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            float maxViewDist = settings_->getMaxViewDistance();
            if (ImGui::InputFloat("Change Max View Distance", &maxViewDist)) {
                settings_->setMaxViewDistance(maxViewDist);
            }

            ImGui::Separator();

            double derivThresh = settings_->getDerivativeThreshold();
            if (ImGui::InputDouble("Adjust Derivative Threshold", &derivThresh)) {
                settings_->setDerivativeThreshold(derivThresh);
            }

            int maxDepth = settings_->getMaxDepth();
            if (ImGui::InputInt("Adjust Depth", &maxDepth)) {
                settings_->setMaxDepth(maxDepth);
            }

            ImGui::Separator();

            bool showAxes = settings_->getShowGridlines();
            if (ImGui::Checkbox("Show Axes", &showAxes)) {
                settings_->setShowGridlines(showAxes);
            }

            bool showLines = settings_->getShowLines();
            if (ImGui::Checkbox("Show Grid Lines", &showLines)) {
                settings_->setShowLines(showLines);
            }

            float minX = settings_->getMinX();
            if (ImGui::InputFloat("Change Minimum X value", &minX)) {
                settings_->setMinX(minX);
            }

            float maxX = settings_->getMaxX();
            if (ImGui::InputFloat("Change Maximum X value", &maxX)) {
                settings_->setMaxX(maxX);
            }

            float minY = settings_->getMinY();
            if (ImGui::InputFloat("Change Minimum Y value", &minY)) {
                settings_->setMinY(minY);
            }

            float maxY = settings_->getMaxY();
            if (ImGui::InputFloat("Change Maximum Y value", &maxY)) {
                settings_->setMaxY(maxY);
            }

            float pointSize = settings_->getPointSize();
            if (ImGui::InputFloat("Set Point Size", &pointSize)) {
                settings_->setPointSize(pointSize);
            }

            ImGui::Separator();

            ImGui::InputText("Filepath for Import (don't forget .mat extension)", 
                           importFilepath_, sizeof(importFilepath_));
            if (ImGui::Button("Import Equations")) {
                if (onImport_ && importFilepath_[0] != '\0') {
                    onImport_(std::string(importFilepath_));
                }
            }

            ImGui::InputText("Filename for Export (no extension required)", 
                           exportFilepath_, sizeof(exportFilepath_));
            if (ImGui::Button("Export Equations")) {
                if (onExport_ && exportFilepath_[0] != '\0') {
                    onExport_(std::string(exportFilepath_));
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void UIController::renderControlsPopup() {
    if (!settings_) {
        return;
    }

    if (settings_->getShowControls()) {
        ImGui::OpenPopup("Controls");
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    if (ImGui::BeginPopupModal("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetItemDefaultFocus();
        ImGui::Text("WASD to move");
        ImGui::Text("Left Control: Down\nSpace: Up");
        ImGui::Text("Q: rotate left\nE: rotate right");
        ImGui::Text("`: toggle keyboard and mouse input");
        ImGui::Text("H: toggle heatmap");
        ImGui::Text("Escape: close program");
        ImGui::Separator();
        
        if (ImGui::Button("Close")) {
            settings_->setShowControls(false);
            mouseFocus_ = true;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::Text("Vertex density might negatively effect performance. Adjust sample size accordingly.");
        ImGui::Text("Be sure to change the colour when working with multiple equations.");

        ImGui::EndPopup();
    }
}

void UIController::renderEquations() {
    if (!equations_ || !settings_) {
        return;
    }

    for (size_t i = 0; i < equations_->size(); ++i) {
        ImGui::PushID(static_cast<int>(i));
        renderEquationInput((*equations_)[i], i);
        ImGui::PopID();
        ImGui::Separator();
    }
}

void UIController::renderEquationInput(Equation& equation, size_t index) {
    if (!settings_) {
        return;
    }

    char buf[256];
    std::strncpy(buf, equation.expression.c_str(), sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    bool needsRender = false;

    if (ImGui::InputText("Equation", buf, sizeof(buf))) {
        equation.expression = buf;
        needsRender = true;
    }

    if (ImGui::ColorEdit3("Colour", equation.color.data())) {
        needsRender = true;
    }

    if (ImGui::SliderInt("Sample Size", &equation.sampleSize, 1, 10000)) {
        needsRender = true;
    }

    float minX = settings_->getMinX();
    float maxX = settings_->getMaxX();
    if (ImGui::SliderFloat("Minimum X", &equation.minX, minX, 0.0f) ||
        ImGui::SliderFloat("Maximum X", &equation.maxX, 1.0f, maxX)) {
        needsRender = true;
    }

    float minY = settings_->getMinY();
    float maxY = settings_->getMaxY();
    if (ImGui::SliderFloat("Minimum Y", &equation.minY, minY, 0.0f) ||
        ImGui::SliderFloat("Maximum Y", &equation.maxY, 1.0f, maxY)) {
        needsRender = true;
    }

    if (ImGui::SliderFloat("Opacity", &equation.opacity, 0.0f, 1.0f)) {
        needsRender = true;
    }

    bool visibilityToggle = ImGui::Checkbox("Toggle Visibility", &equation.isVisible);
    bool toggle3D = ImGui::Checkbox("Toggle 3D", &equation.is3D);
    bool useHeatmap = settings_->getUseHeatmap();
    bool heatmapToggle = ImGui::Checkbox("Toggle Heatmap", &useHeatmap);
    if (heatmapToggle) {
        settings_->setUseHeatmap(useHeatmap);
    }
    bool meshToggle = ImGui::Checkbox("Toggle Mesh (might not work for all functions)", &equation.isMesh);

    if (visibilityToggle || toggle3D || heatmapToggle || meshToggle) {
        needsRender = true;
    }

    if (ImGui::Button("Remove Equation")) {
        if (onEquationRemove_) {
            onEquationRemove_(index);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Render") || needsRender) {
        if (onEquationRender_) {
            onEquationRender_(equation, index);
        }
    }
}

void UIController::renderPoints() {
    if (!points_) {
        return;
    }

    for (size_t i = 0; i < points_->size(); ++i) {
        ImGui::PushID(static_cast<int>(i));
        renderPointInput((*points_)[i], i);
        ImGui::PopID();
        ImGui::Separator();
    }
}

void UIController::renderPointInput(Point& point, size_t index) {
    float pos[3] = {point.position.x, point.position.y, point.position.z};
    
    if (ImGui::InputFloat3("Point", pos)) {
        point.position = glm::vec3(pos[0], pos[1], pos[2]);
    }

    ImGui::ColorEdit3("Colour", point.color.data());

    if (ImGui::Button("Remove Point")) {
        if (onPointRemove_) {
            onPointRemove_(index);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Render")) {
        if (onPointRender_) {
            onPointRender_(point, index);
        }
    }
}

// Callback setters
void UIController::setOnEquationRender(std::function<void(Equation&, size_t)> callback) {
    onEquationRender_ = callback;
}

void UIController::setOnEquationRemove(std::function<void(size_t)> callback) {
    onEquationRemove_ = callback;
}

void UIController::setOnPointRender(std::function<void(Point&, size_t)> callback) {
    onPointRender_ = callback;
}

void UIController::setOnPointRemove(std::function<void(size_t)> callback) {
    onPointRemove_ = callback;
}

void UIController::setOnEquationAdd(std::function<void()> callback) {
    onEquationAdd_ = callback;
}

void UIController::setOnPointAdd(std::function<void()> callback) {
    onPointAdd_ = callback;
}

void UIController::setOnImport(std::function<void(const std::string&)> callback) {
    onImport_ = callback;
}

void UIController::setOnExport(std::function<void(const std::string&)> callback) {
    onExport_ = callback;
}

} // namespace graphgl

