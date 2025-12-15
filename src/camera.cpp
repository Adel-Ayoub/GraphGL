#include "camera.h"
#include <algorithm>

namespace graphgl {

Camera::Camera(const glm::vec3& position)
    : position_(position)
    , orientation_(glm::quat(0, 0, 0, -1))
    , rightAngle_(0.0f)
    , upAngle_(0.0f)
    , rollAngle_(0.0f)
    , movementSpeed_(DEFAULT_SPEED)
    , mouseSensitivity_(DEFAULT_SENSITIVITY)
    , zoom_(DEFAULT_ZOOM)
    , rollSpeed_(DEFAULT_ROLL_SPEED)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ)
    : Camera(glm::vec3(posX, posY, posZ))
{
}

Camera::Camera(const glm::vec3& position, const glm::quat& orientation)
    : position_(position)
    , orientation_(orientation)
    , rightAngle_(0.0f)
    , upAngle_(0.0f)
    , rollAngle_(0.0f)
    , movementSpeed_(DEFAULT_SPEED)
    , mouseSensitivity_(DEFAULT_SENSITIVITY)
    , zoom_(DEFAULT_ZOOM)
    , rollSpeed_(DEFAULT_ROLL_SPEED)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    glm::quat reverseOrientation = glm::conjugate(orientation_);
    glm::mat4 rotation = glm::mat4_cast(reverseOrientation);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), -position_);
    
    return rotation * translation;
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed_ * deltaTime;
    
    glm::vec3 front = getFrontVector();
    glm::vec3 right = getRightVector();
    glm::vec3 up = getUpVector();
    
    switch (direction) {
        case CameraMovement::FORWARD:
            position_ += front * velocity;
            break;
        case CameraMovement::BACKWARD:
            position_ -= front * velocity;
            break;
        case CameraMovement::LEFT:
            position_ -= right * velocity;
            break;
        case CameraMovement::RIGHT:
            position_ += right * velocity;
            break;
        case CameraMovement::UP:
            position_ += up * velocity;
            break;
        case CameraMovement::DOWN:
            position_ -= up * velocity;
            break;
        case CameraMovement::ROLL_LEFT:
            rollAngle_ -= deltaTime * rollSpeed_;
            updateCameraVectors();
            break;
        case CameraMovement::ROLL_RIGHT:
            rollAngle_ += deltaTime * rollSpeed_;
            updateCameraVectors();
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity_;
    yoffset *= mouseSensitivity_;
    
    rightAngle_ += xoffset;
    upAngle_ += yoffset;
    
    if (constrainPitch) {
        upAngle_ = std::clamp(upAngle_, MIN_PITCH, MAX_PITCH);
    }
    
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    zoom_ -= yoffset;
    setZoom(zoom_);
}

void Camera::reset(const glm::vec3& position) {
    position_ = position;
    rightAngle_ = 0.0f;
    upAngle_ = 0.0f;
    rollAngle_ = 0.0f;
    zoom_ = DEFAULT_ZOOM;
    orientation_ = glm::quat(0, 0, 0, -1);
    updateCameraVectors();
}

void Camera::setZoom(float zoom) {
    zoom_ = std::clamp(zoom, MIN_ZOOM, MAX_ZOOM);
}

void Camera::updateCameraVectors() {
    glm::quat aroundY = glm::angleAxis(glm::radians(-rightAngle_), glm::vec3(0, 1, 0));
    glm::quat aroundX = glm::angleAxis(glm::radians(upAngle_), glm::vec3(1, 0, 0));
    glm::quat roll = glm::angleAxis(glm::radians(rollAngle_), glm::vec3(0.0f, 0.0f, 1.0f));
    
    orientation_ = roll * aroundY * aroundX;
    orientation_ = glm::normalize(orientation_);
}

glm::vec3 Camera::getFrontVector() const {
    glm::quat qF = orientation_ * glm::quat(0, 0, 0, -1) * glm::conjugate(orientation_);
    return glm::vec3(qF.x, qF.y, qF.z);
}

glm::vec3 Camera::getRightVector() const {
    glm::vec3 front = getFrontVector();
    return glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
}

glm::vec3 Camera::getUpVector() const {
    glm::vec3 front = getFrontVector();
    glm::vec3 right = getRightVector();
    return glm::normalize(glm::cross(right, front));
}

} // namespace graphgl

