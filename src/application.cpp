#include "application.h"
#include <iostream>
#include <stdexcept>

namespace graphgl {

Application::Application()
    : window_(nullptr)
    , width_(1280)
    , height_(720)
    , initialized_(false)
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

    // Set callbacks
    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
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

    initialized_ = true;
    return true;
}

void Application::run() {
    if (!initialized_ || !window_) {
        std::cerr << "Application not initialized" << std::endl;
        return;
    }

    while (!glfwWindowShouldClose(window_)) {
        // Process input
        glfwPollEvents();

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    
    // Update application dimensions if we have access to the instance
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->width_ = width;
        app->height_ = height;
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

