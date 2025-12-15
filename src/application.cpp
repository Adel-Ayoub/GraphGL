#include "application.h"
#include "camera.h"
#include "shader.h"
#include "renderer.h"
#include "equation_renderer.h"
#include "grid_renderer.h"
#include "ui_controller.h"
#include "settings.h"
#include "data_manager.h"
#include "equation_parser.h"
#include "equation_generator.h"
#include "equation.h"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_opengl3.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace graphgl {

Application::Application()
    : window_(nullptr)
    , width_(1280)
    , height_(720)
    , initialized_(false)
    ,     lastX_(640.0f)
    , lastY_(360.0f)
    , firstMouse_(true)
    , mouseFocus_(false)
    , deltaTime_(0.0f)
    , lastFrame_(0.0f)
{
}

Application::~Application() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

bool Application::initialize(int width, int height, const char* title) {
    if (initialized_) {
        std::cerr << "Application already initialized" << std::endl;
        return false;
    }

    width_ = width;
    height_ = height;
    lastX_ = width_ / 2.0f;
    lastY_ = height_ / 2.0f;

    // Set error callback before initializing GLFW
    glfwSetErrorCallback(errorCallback);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    window_ = glfwCreateWindow(width_, height_, title, nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Set context
    glfwMakeContextCurrent(window_);

    // Set callbacks (we forward to ImGui ourselves; ImGui install_callbacks is false)
    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
    glfwSetCursorPosCallback(window_, mouseCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetCharCallback(window_, charCallback);
    glfwSetKeyCallback(window_, keyCallback);
    glfwSetWindowUserPointer(window_, this);

    // Initialize GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window_);
        glfwTerminate();
        return false;
    }

    // Initialize OpenGL settings
    initializeOpenGL();

    // Create components
    settings_ = std::make_unique<Settings>();
    settings_->setWidth(width_);
    settings_->setHeight(height_);

    camera_ = std::make_unique<Camera>(glm::vec3(0.0f, 6.0f, 12.0f));

    try {
        shader_ = std::make_unique<Shader>("shaders/shader.vs", "shaders/shader.fs");
    } catch (const std::exception& e) {
        std::cerr << "Failed to load shaders: " << e.what() << std::endl;
        return false;
    }

    renderer_ = std::make_unique<Renderer>();
    renderer_->initialize();

    equationRenderer_ = std::make_unique<EquationRenderer>();
    equationRenderer_->initialize();

    gridRenderer_ = std::make_unique<GridRenderer>();
    gridRenderer_->initialize();

    uiController_ = std::make_unique<UIController>();
    if (!uiController_->initialize(window_)) {
        std::cerr << "Failed to initialize UI controller" << std::endl;
        return false;
    }

    dataManager_ = std::make_unique<DataManager>();
    equationParser_ = std::make_unique<EquationParser>();
    equationGenerator_ = std::make_unique<EquationGenerator>();

    // Set up UI data references
    uiController_->setEquations(&equations_);
    uiController_->setPoints(&points_);
    uiController_->setSettings(settings_.get());
    uiController_->setCamera(camera_.get());

    // Set up UI callbacks
    setupUICallbacks();

    // Add initial equation
    equations_.emplace_back();

    // Set initial input mode - start with UI focus (cursor visible)
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseFocus_ = false;

    initialized_ = true;
    return true;
}

void Application::run() {
    if (!initialized_ || !window_) {
        std::cerr << "Application not initialized" << std::endl;
        return;
    }

    while (!glfwWindowShouldClose(window_)) {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime_ = currentFrame - lastFrame_;
        lastFrame_ = currentFrame;

        // Poll events first to update input state
        glfwPollEvents();

        // Process input (before ImGui processes)
        processInput();

        // Render (which includes ImGui processing)
        render();

        // Swap buffers
        glfwSwapBuffers(window_);
    }
}

bool Application::shouldClose() const {
    if (!window_) {
        return true;
    }
    return glfwWindowShouldClose(window_) != 0;
}

void Application::processInput() {
    // ESCAPE always works regardless of focus mode
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
        return;
    }

    // Always allow keyboard movement (even when UI has focus)
    handleKeyboardInput();
}

void Application::handleKeyboardInput() {
    if (!camera_) {
        return;
    }

    // Camera movement keys
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::FORWARD, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::BACKWARD, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::LEFT, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::RIGHT, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::ROLL_LEFT, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::ROLL_RIGHT, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::UP, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera_->processKeyboard(CameraMovement::DOWN, deltaTime_);
    }
    if (glfwGetKey(window_, GLFW_KEY_I) == GLFW_PRESS) {
        camera_->reset(glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void Application::render() {
    if (!renderer_ || !shader_ || !camera_ || !settings_) {
        return;
    }

    // Clear screen
    renderer_->clear();

    // Start ImGui frame (before setting WantCaptureKeyboard like original)
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set ImGui capture state AFTER NewFrame
    ImGuiIO& io = ImGui::GetIO();
    if (!mouseFocus_) {
        io.WantCaptureMouse = true;
        io.WantCaptureKeyboard = true;
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        io.WantCaptureMouse = false;
        io.WantCaptureKeyboard = false;
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    // Render UI
    uiController_->renderMainWindow();
    uiController_->renderControlsPopup();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Use shader
    shader_->use();

    // Set up matrices
    glm::mat4 model = glm::mat4(1.0f);
    shader_->setMat4("model", model);

    glm::mat4 projection = glm::perspective(
        glm::radians(camera_->getZoom()),
        static_cast<float>(width_) / static_cast<float>(height_),
        0.1f,
        settings_->getMaxViewDistance()
    );
    shader_->setMat4("projection", projection);

    glm::mat4 view = camera_->getViewMatrix();
    shader_->setMat4("view", view);

    // Set shader uniforms
    shader_->setFloat("point_size", settings_->getPointSize());
    shader_->setFloat("point_opacity", 1.0f);

    // Render grid (behind everything)
    glDepthMask(GL_FALSE);
    if (settings_->getShowLines()) {
        gridRenderer_->renderGridLines(*shader_);
    }
    if (settings_->getShowGridlines()) {
        gridRenderer_->renderAxes(*shader_);
    }
    glDepthMask(GL_TRUE);

    // Render equations
    equationRenderer_->updateVertices(equations_, points_);
    
    // Set color and opacity for each equation
    for (const auto& equation : equations_) {
        if (equation.isVisible) {
            glm::vec3 color(equation.color[0], equation.color[1], equation.color[2]);
            shader_->setVec3("color", color);
            shader_->setFloat("point_opacity", equation.opacity);
        }
    }

    equationRenderer_->render(
        *shader_,
        settings_->getUseHeatmap(),
        settings_->getMinHeight(),
        settings_->getMaxHeight()
    );
}

void Application::setupUICallbacks() {
    uiController_->setOnEquationRender([this](Equation& eq, size_t idx) {
        onEquationRender(eq, idx);
    });

    uiController_->setOnEquationRemove([this](size_t idx) {
        onEquationRemove(idx);
    });

    uiController_->setOnPointRender([this](Point& pt, size_t idx) {
        onPointRender(pt, idx);
    });

    uiController_->setOnPointRemove([this](size_t idx) {
        onPointRemove(idx);
    });

    uiController_->setOnEquationAdd([this]() {
        onEquationAdd();
    });

    uiController_->setOnPointAdd([this]() {
        onPointAdd();
    });

    uiController_->setOnImport([this](const std::string& filename) {
        onImport(filename);
    });

    uiController_->setOnExport([this](const std::string& filename) {
        onExport(filename);
    });
}

void Application::onEquationRender(Equation& equation, size_t /* index */) {
    if (!equationParser_ || !equationGenerator_ || !settings_) {
        return;
    }

    // Parse expression
    if (!equationParser_->parseExpression(equation.expression, equation.is3D)) {
        std::cerr << "Failed to parse equation: " << equationParser_->getErrorMessage() << std::endl;
        return;
    }

    // Generate vertices
    equationGenerator_->generateVertices(
        equation,
        *equationParser_,
        settings_->getMaxDepth(),
        settings_->getDerivativeThreshold()
    );

    // Update height tracking
    settings_->setMinHeight(equationGenerator_->getMinHeight());
    settings_->setMaxHeight(equationGenerator_->getMaxHeight());

    // Rerender
    rerender();
}

void Application::onEquationRemove(size_t index) {
    if (index < equations_.size()) {
        equations_.erase(equations_.begin() + index);
        rerender();
    }
}

void Application::onPointRender(Point& point, size_t index) {
    (void)index; // Unused
    updatePointVertices(point);
    rerender();
}

void Application::onPointRemove(size_t index) {
    if (index < points_.size()) {
        points_.erase(points_.begin() + index);
        rerender();
    }
}

void Application::onEquationAdd() {
    equations_.emplace_back();
}

void Application::onPointAdd() {
    points_.emplace_back();
}

void Application::onImport(const std::string& filename) {
    if (!dataManager_) {
        return;
    }

    std::vector<Equation> importedEquations;
    std::vector<Point> importedPoints;

    if (dataManager_->importData(filename, importedEquations, importedPoints)) {
        equations_.insert(equations_.end(), importedEquations.begin(), importedEquations.end());
        points_.insert(points_.end(), importedPoints.begin(), importedPoints.end());
        rerender();
    }
}

void Application::onExport(const std::string& filename) {
    if (!dataManager_) {
        return;
    }

    dataManager_->exportData(filename, equations_, points_);
}

void Application::updateEquationVertices(Equation& equation) {
    if (!equationParser_ || !equationGenerator_ || !settings_) {
        return;
    }

    if (!equationParser_->parseExpression(equation.expression, equation.is3D)) {
        return;
    }

    equationGenerator_->generateVertices(
        equation,
        *equationParser_,
        settings_->getMaxDepth(),
        settings_->getDerivativeThreshold()
    );
}

void Application::updatePointVertices(Point& point) {
    point.vertexData.clear();
    point.vertexData.push_back(point.position);
    point.vertexData.push_back(glm::vec3(point.color[0], point.color[1], point.color[2]));
}

void Application::rerender() {
    if (equationRenderer_) {
        equationRenderer_->updateVertices(equations_, points_);
    }
}

// Static callback implementations
void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->width_ = width;
        app->height_ = height;
        if (app->settings_) {
            app->settings_->setWidth(width);
            app->settings_->setHeight(height);
        }
    }
}

void Application::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    ImGui_ImplGlfw_CursorPosCallback(window, xposIn, yposIn);
    if (!app || !app->camera_ || !app->mouseFocus_) {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (app->firstMouse_) {
        app->lastX_ = xpos;
        app->lastY_ = ypos;
        app->firstMouse_ = false;
    }

    float xoffset = xpos - app->lastX_;
    float yoffset = app->lastY_ - ypos;

    app->lastX_ = xpos;
    app->lastY_ = ypos;

    app->camera_->processMouseMovement(xoffset, yoffset);
}

void Application::scrollCallback(GLFWwindow* window, double /* xoffset */, double yoffset) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    ImGui_ImplGlfw_ScrollCallback(window, 0.0, yoffset);
    if (app && app->camera_) {
        app->camera_->processMouseScroll(static_cast<float>(yoffset));
    }
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    (void)window;
    (void)button;
    (void)action;
    (void)mods;
}

void Application::charCallback(GLFWwindow* window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    (void)scancode; // Unused
    (void)mods; // Unused

    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || !app->settings_ || !app->uiController_) {
        return;
    }

    // Only act on key press (avoids rapid toggling)
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_GRAVE_ACCENT || key == GLFW_KEY_TAB) { // allow TAB as a fallback toggle
            app->mouseFocus_ = !app->mouseFocus_;
            app->uiController_->setMouseFocus(app->mouseFocus_);
            app->firstMouse_ = true;
            glfwSetInputMode(window, GLFW_CURSOR, app->mouseFocus_ ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        } else if (key == GLFW_KEY_H) {
            app->settings_->setUseHeatmap(!app->settings_->getUseHeatmap());
        }
    }
}

void Application::errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void Application::initializeOpenGL() const {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    // Set viewport
    glViewport(0, 0, width_, height_);
}

} // namespace graphgl
