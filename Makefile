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
DEPS = $(OBJECTS:.o=.d)

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

# Detect OS
UNAME_S := $(shell uname -s)

# Library paths and libraries - platform specific
ifeq ($(UNAME_S),Darwin)
    # macOS
    LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib
    LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else ifeq ($(UNAME_S),Linux)
    # Linux
    LDFLAGS = 
    LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
else
    # Windows (MinGW/MSYS2)
    LDFLAGS = 
    LIBS = -lglfw3 -lopengl32 -lgdi32
endif

# pkg-config for GLFW (if available)
PKG_CONFIG = pkg-config
GLFW_CFLAGS = $(shell $(PKG_CONFIG) --cflags glfw3 2>/dev/null || echo "")
GLFW_LIBS = $(shell $(PKG_CONFIG) --libs glfw3 2>/dev/null || echo "")

# Override with pkg-config if available
ifneq ($(GLFW_CFLAGS),)
    INCLUDES += $(GLFW_CFLAGS)
endif
ifneq ($(GLFW_LIBS),)
    ifeq ($(UNAME_S),Darwin)
        LIBS = $(GLFW_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else ifeq ($(UNAME_S),Linux)
        LIBS = $(GLFW_LIBS) -lGL -lX11 -lpthread -lXrandr -lXi -ldl
    else
        LIBS = $(GLFW_LIBS) -lopengl32 -lgdi32
    endif
endif

# Set flags based on build mode
ifeq ($(BUILD_MODE),release)
    CXXFLAGS += $(RELEASE_FLAGS)
else
    CXXFLAGS += $(DEBUG_FLAGS)
endif

# Google Test
GTEST_DIR = $(LIB_DIR)/googletest/googletest
GTEST_INCLUDES = -I$(GTEST_DIR)/include -I$(GTEST_DIR)
GTEST_OBJ = $(BUILD_DIR)/gtest-all.o
GTEST_MAIN_OBJ = $(BUILD_DIR)/gtest_main.o

# Test files
TEST_DIR = tests
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

# Only the non-OpenGL source objects needed by tests.
TEST_SRC_OBJECTS = $(BUILD_DIR)/settings.o \
                   $(BUILD_DIR)/equation_parser.o \
                   $(BUILD_DIR)/equation_generator.o \
                   $(BUILD_DIR)/data_manager.o

# Executable name
TARGET = $(BUILD_DIR)/graphgl
TEST_TARGET = $(BUILD_DIR)/graphgl_tests

# Default target
all: $(TARGET)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link executable and copy runtime resources.
$(TARGET): $(BUILD_DIR) $(OBJECTS) $(GLAD_OBJ) $(IMGUI_OBJECTS)
	$(CXX) $(OBJECTS) $(GLAD_OBJ) $(IMGUI_OBJECTS) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@cp -r $(SHADER_DIR) $(BUILD_DIR)/$(SHADER_DIR)
	@echo "Build complete: $(TARGET)"

# Compile source files (with automatic header dependency tracking)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

-include $(DEPS)

# Compile GLAD (C file, use CC with CFLAGS)
$(GLAD_OBJ): $(GLAD_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile ImGui sources
$(BUILD_DIR)/imgui_%.o: $(LIB_DIR)/imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Install target
PREFIX ?= /usr/local
install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/graphgl
	install -d $(PREFIX)/share/graphgl/shaders
	install -m 644 $(SHADER_DIR)/* $(PREFIX)/share/graphgl/shaders/
	@echo "Installed to $(PREFIX)"

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
	@echo "  test      - Build and run unit tests"
	@echo "  clean     - Remove build artifacts"
	@echo "  run       - Build and run the program"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              - Build in debug mode"
	@echo "  make BUILD_MODE=release - Build in release mode"
	@echo "  make clean        - Clean build files"

# --- Test targets ---

# Compile Google Test library.
$(GTEST_OBJ): $(GTEST_DIR)/src/gtest-all.cc
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDES) -c $< -o $@

$(GTEST_MAIN_OBJ): $(GTEST_DIR)/src/gtest_main.cc
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDES) -c $< -o $@

# Compile test source files.
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GTEST_INCLUDES) -MMD -MP -c $< -o $@

# Link test runner. Tests don't use OpenGL/GLFW, so we skip GLAD, ImGui, and GL libs.
$(TEST_TARGET): $(BUILD_DIR) $(TEST_SRC_OBJECTS) $(TEST_OBJECTS) $(GTEST_OBJ) $(GTEST_MAIN_OBJ)
	$(CXX) $(TEST_SRC_OBJECTS) $(TEST_OBJECTS) $(GTEST_OBJ) $(GTEST_MAIN_OBJ) -lpthread -o $(TEST_TARGET)
	@echo "Test build complete: $(TEST_TARGET)"

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean install run help test

