#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace graphgl {

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

    // Callback functions (static wrappers for GLFW callbacks)
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);

    // Initialize OpenGL settings
    void initializeOpenGL() const;
};

} // namespace graphgl

