#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace graphgl {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : id_(0)
{
    // Load shader source code
    std::string vertexCode = loadShaderFile(vertexPath);
    std::string fragmentCode = loadShaderFile(fragmentPath);
    
    // Compile shaders
    unsigned int vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    
    // Create shader program
    id_ = createProgram(vertexShader, fragmentShader);
    
    // Clean up individual shaders (they're linked into the program)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    if (id_ == 0) {
        throw std::runtime_error("Failed to create shader program");
    }
}

Shader::~Shader() {
    if (id_ != 0) {
        glDeleteProgram(id_);
        id_ = 0;
    }
}

Shader::Shader(Shader&& other) noexcept
    : id_(other.id_)
{
    other.id_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (id_ != 0) {
            glDeleteProgram(id_);
        }
        id_ = other.id_;
        other.id_ = 0;
    }
    return *this;
}

void Shader::use() const {
    if (id_ == 0) {
        std::cerr << "Warning: Attempting to use invalid shader" << std::endl;
        return;
    }
    glUseProgram(id_);
}

std::string Shader::loadShaderFile(const std::string& filepath) const {
    std::ifstream file;
    std::stringstream stream;
    
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        file.open(filepath);
        stream << file.rdbuf();
        file.close();
        return stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::string errorMsg = "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " + filepath + " - " + e.what();
        std::cerr << errorMsg << std::endl;
        throw std::runtime_error(errorMsg);
    }
}

unsigned int Shader::compileShader(const std::string& source, unsigned int type) const {
    unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        throw std::runtime_error("Failed to create shader object");
    }
    
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);
    
    std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
    checkCompileErrors(shader, shaderType);
    
    return shader;
}

unsigned int Shader::createProgram(unsigned int vertexShader, unsigned int fragmentShader) const {
    unsigned int program = glCreateProgram();
    if (program == 0) {
        throw std::runtime_error("Failed to create shader program");
    }
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    checkLinkErrors(program);
    
    return program;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type) const {
    int success;
    char infoLog[1024];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::string errorMsg = "ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog;
        std::cerr << errorMsg << std::endl;
        throw std::runtime_error(errorMsg);
    }
}

void Shader::checkLinkErrors(unsigned int program) const {
    int success;
    char infoLog[1024];
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::string errorMsg = "ERROR::PROGRAM_LINKING_ERROR\n" + std::string(infoLog);
        std::cerr << errorMsg << std::endl;
        throw std::runtime_error(errorMsg);
    }
}

int Shader::getUniformLocation(const std::string& name) const {
    int location = glGetUniformLocation(id_, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader" << std::endl;
    }
    return location;
}

void Shader::setBool(const std::string& name, bool value) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform1i(loc, static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform1i(loc, value);
}

void Shader::setFloat(const std::string& name, float value) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform1f(loc, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform2f(loc, x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform3f(loc, x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform4f(loc, x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    int loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

} // namespace graphgl

