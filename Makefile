# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Build mode (debug or release)
BUILD_MODE ?= debug

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
SHADER_DIR = shaders
LIB_DIR = lib

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# GLAD source
GLAD_SRC = $(LIB_DIR)/glad/glad.c
GLAD_OBJ = $(BUILD_DIR)/glad.o

# ImGui sources
IMGUI_SOURCES = $(LIB_DIR)/imgui/imgui.cpp \
                $(LIB_DIR)/imgui/imgui_demo.cpp \
                $(LIB_DIR)/imgui/imgui_draw.cpp \
                $(LIB_DIR)/imgui/imgui_tables.cpp \
                $(LIB_DIR)/imgui/imgui_widgets.cpp \
                $(LIB_DIR)/imgui/backends/imgui_impl_glfw.cpp \
                $(LIB_DIR)/imgui/backends/imgui_impl_opengl3.cpp

IMGUI_OBJECTS = $(patsubst $(LIB_DIR)/imgui/%.cpp,$(BUILD_DIR)/imgui_%.o,$(IMGUI_SOURCES))

# Include directories
INCLUDES = -I$(INCLUDE_DIR) \
           -I$(LIB_DIR)/glad/include \
           -I$(LIB_DIR)/imgui \
           -I$(LIB_DIR)/exprtk \
           -I$(LIB_DIR)/stb \
           -I$(LIB_DIR)

# Library paths and libraries
LDFLAGS = -L/opt/homebrew/lib
LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# pkg-config for GLFW (if available)
PKG_CONFIG = pkg-config
GLFW_CFLAGS = $(shell $(PKG_CONFIG) --cflags glfw3 2>/dev/null || echo "")
GLFW_LIBS = $(shell $(PKG_CONFIG) --libs glfw3 2>/dev/null || echo "-lglfw")

# Override with pkg-config if available
ifneq ($(GLFW_CFLAGS),)
    INCLUDES += $(GLFW_CFLAGS)
endif
ifneq ($(GLFW_LIBS),)
    LIBS = $(GLFW_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Set flags based on build mode
ifeq ($(BUILD_MODE),release)
    CXXFLAGS += $(RELEASE_FLAGS)
else
    CXXFLAGS += $(DEBUG_FLAGS)
endif

# Executable name
TARGET = $(BUILD_DIR)/graphgl

# Default target
all: $(TARGET)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link executable
$(TARGET): $(BUILD_DIR) $(OBJECTS) $(GLAD_OBJ) $(IMGUI_OBJECTS)
	$(CXX) $(OBJECTS) $(GLAD_OBJ) $(IMGUI_OBJECTS) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile GLAD
$(GLAD_OBJ): $(GLAD_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile ImGui sources
$(BUILD_DIR)/imgui_%.o: $(LIB_DIR)/imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Install target (optional)
install: $(TARGET)
	@echo "Install target not implemented"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Help target
help:
	@echo "GraphGL Makefile"
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the project (default, debug mode)"
	@echo "  debug     - Build in debug mode"
	@echo "  release   - Build in release mode"
	@echo "  clean     - Remove build artifacts"
	@echo "  run       - Build and run the program"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              - Build in debug mode"
	@echo "  make BUILD_MODE=release - Build in release mode"
	@echo "  make clean        - Clean build files"

.PHONY: all clean install run help

