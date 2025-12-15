#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

namespace graphgl {

// Forward declarations
class Camera;
class Shader;
class Renderer;
class EquationRenderer;
class GridRenderer;
class UIController;
class Settings;
class DataManager;
class EquationParser;
class EquationGenerator;
struct Equation;
struct Point;

class Application {
public:
    Application();
    ~Application();

    // Delete copy constructor and assignment operator
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Initialize the application
    bool initialize(int width = 1280, int height = 720, const char* title = "GraphGL");

    // Run the main loop
    void run();

    // Check if the application should close
    bool shouldClose() const;

    // Get the GLFW window handle
    GLFWwindow* getWindow() const { return window_; }

    // Get window dimensions
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    GLFWwindow* window_;
    int width_;
    int height_;
    bool initialized_;

    // Core components
    std::unique_ptr<Settings> settings_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<Shader> shader_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<EquationRenderer> equationRenderer_;
    std::unique_ptr<GridRenderer> gridRenderer_;
    std::unique_ptr<UIController> uiController_;
    std::unique_ptr<DataManager> dataManager_;
    std::unique_ptr<EquationParser> equationParser_;
    std::unique_ptr<EquationGenerator> equationGenerator_;

    // Data
    std::vector<Equation> equations_;
    std::vector<Point> points_;

    // Input state
    float lastX_;
    float lastY_;
    bool firstMouse_;
    bool mouseFocus_;
    float deltaTime_;
    float lastFrame_;

    // Callback functions (static wrappers for GLFW callbacks)
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void errorCallback(int error, const char* description);

    // Initialize OpenGL settings
    void initializeOpenGL() const;

    // Input handling
    void processInput();
    void handleKeyboardInput();

    // Rendering
    void render();

    // UI callbacks
    void setupUICallbacks();
    void onEquationRender(Equation& equation, size_t index);
    void onEquationRemove(size_t index);
    void onPointRender(Point& point, size_t index);
    void onPointRemove(size_t index);
    void onEquationAdd();
    void onPointAdd();
    void onImport(const std::string& filename);
    void onExport(const std::string& filename);

    // Helper methods
    void updateEquationVertices(Equation& equation);
    void updatePointVertices(Point& point);
    void rerender();
};

} // namespace graphgl
