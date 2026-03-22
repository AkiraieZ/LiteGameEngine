#include "Camera.h"
#include <GLFW/glfw3.h>

namespace LGE {

Camera::Camera(const glm::vec3& position)
    : m_Position(position), m_WorldUp(0.0f, 1.0f, 0.0f),
      m_Yaw(-90.0f), m_Pitch(0.0f), m_Fov(45.0f),
      m_Near(0.1f), m_Far(1000.0f), m_AspectRatio(16.0f / 9.0f),
      m_MovementSpeed(2.5f), m_MouseSensitivity(0.1f),
      m_ViewMatrix(1.0f), m_ProjectionMatrix(1.0f), m_ViewProjectionMatrix(1.0f) {
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::OnUpdate(float deltaTime) {
}

void Camera::UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() {
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::UpdateProjectionMatrix() {
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_Near, m_Far);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;
    
    m_Yaw += xoffset;
    m_Pitch += yoffset;
    
    if (constrainPitch) {
        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;
    }
    
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    m_Fov -= yoffset;
    if (m_Fov < 1.0f) m_Fov = 1.0f;
    if (m_Fov > 45.0f) m_Fov = 45.0f;
    UpdateProjectionMatrix();
}

void Camera::ProcessKeyboard(int key, float deltaTime) {
    float velocity = m_MovementSpeed * deltaTime;
    if (key == GLFW_KEY_W) m_Position += m_Front * velocity;
    if (key == GLFW_KEY_S) m_Position -= m_Front * velocity;
    if (key == GLFW_KEY_A) m_Position -= m_Right * velocity;
    if (key == GLFW_KEY_D) m_Position += m_Right * velocity;
    if (key == GLFW_KEY_SPACE) m_Position += m_WorldUp * velocity;
    if (key == GLFW_KEY_LEFT_SHIFT) m_Position -= m_WorldUp * velocity;
    UpdateViewMatrix();
}

}
