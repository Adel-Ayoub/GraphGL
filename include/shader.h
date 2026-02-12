#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <stdexcept>

namespace graphgl {

class Shader {
public:
    // Constructor - loads and compiles shaders from files
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Destructor - cleans up OpenGL resources
    ~Shader();
    
    // Delete copy constructor and assignment (use move instead)
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Move constructor and assignment
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    // Use the shader program
    void use() const;
    
    // Uniform setters
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    
    // Get the shader program ID
    unsigned int getId() const { return id_; }
    
    [[nodiscard]] bool isValid() const { return id_ != 0; }

private:
    unsigned int id_;
    
    // Helper functions
    std::string loadShaderFile(const std::string& filepath) const;
    unsigned int compileShader(const std::string& source, unsigned int type) const;
    unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) const;
    void checkCompileErrors(unsigned int shader, const std::string& type) const;
    void checkLinkErrors(unsigned int program) const;
    int getUniformLocation(const std::string& name) const;
};

} // namespace graphgl

