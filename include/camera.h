#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace graphgl {

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROLL_LEFT,
    ROLL_RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    // Constants
    static constexpr float DEFAULT_SPEED = 10.0f;
    static constexpr float DEFAULT_ROLL_SPEED = 7.5f;
    static constexpr float DEFAULT_SENSITIVITY = 0.1f;
    static constexpr float DEFAULT_ZOOM = 45.0f;
    static constexpr float MIN_ZOOM = 1.0f;
    static constexpr float MAX_ZOOM = 45.0f;
    static constexpr float MAX_PITCH = 89.0f;
    static constexpr float MIN_PITCH = -89.0f;

    // Constructors
    explicit Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));
    Camera(float posX, float posY, float posZ);
    Camera(const glm::vec3& position, const glm::quat& orientation);

    // Getters
    glm::vec3 getPosition() const { return position_; }
    glm::quat getOrientation() const { return orientation_; }
    float getMovementSpeed() const { return movementSpeed_; }
    float getMouseSensitivity() const { return mouseSensitivity_; }
    float getZoom() const { return zoom_; }
    float getRollSpeed() const { return rollSpeed_; }

    // Setters
    void setPosition(const glm::vec3& position) { position_ = position; }
    void setMovementSpeed(float speed) { movementSpeed_ = speed; }
    void setMouseSensitivity(float sensitivity) { mouseSensitivity_ = sensitivity; }
    void setZoom(float zoom);
    void setRollSpeed(float speed) { rollSpeed_ = speed; }

    // Camera operations
    glm::mat4 getViewMatrix() const;
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

private:
    glm::vec3 position_;
    glm::quat orientation_;
    
    float rightAngle_;
    float upAngle_;
    float rollAngle_;
    
    float movementSpeed_;
    float mouseSensitivity_;
    float zoom_;
    float rollSpeed_;

    void updateCameraVectors();
    glm::vec3 getFrontVector() const;
    glm::vec3 getRightVector() const;
    glm::vec3 getUpVector() const;
};

} // namespace graphgl

