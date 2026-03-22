#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace LGE {

class Camera {
public:
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f));
    ~Camera() = default;
    
    void OnUpdate(float deltaTime);
    
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    
    const glm::vec3& GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec3& position) { m_Position = position; UpdateViewMatrix(); }
    
    const glm::vec3& GetFront() const { return m_Front; }
    const glm::vec3& GetUp() const { return m_Up; }
    const glm::vec3& GetRight() const { return m_Right; }
    
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    void SetYaw(float yaw) { m_Yaw = yaw; UpdateCameraVectors(); }
    void SetPitch(float pitch) { m_Pitch = pitch; UpdateCameraVectors(); }
    
    float GetFov() const { return m_Fov; }
    void SetFov(float fov) { m_Fov = fov; UpdateProjectionMatrix(); }
    
    float GetNear() const { return m_Near; }
    float GetFar() const { return m_Far; }
    void SetNear(float nearVal) { m_Near = nearVal; UpdateProjectionMatrix(); }
    void SetFar(float farVal) { m_Far = farVal; UpdateProjectionMatrix(); }
    
    float GetAspectRatio() const { return m_AspectRatio; }
    void SetAspectRatio(float aspect) { m_AspectRatio = aspect; UpdateProjectionMatrix(); }
    
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void ProcessKeyboard(int key, float deltaTime);
    
private:
    void UpdateCameraVectors();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    
    float m_Yaw;
    float m_Pitch;
    float m_Fov;
    float m_Near;
    float m_Far;
    float m_AspectRatio;
    float m_MovementSpeed;
    float m_MouseSensitivity;
    
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewProjectionMatrix;
};

}
