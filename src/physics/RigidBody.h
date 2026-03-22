#pragma once

#include <glm/glm.hpp>

namespace LGE {

enum class BodyType {
    STATIC,
    DYNAMIC,
    KINEMATIC
};

enum class ColliderType {
    NONE,
    BOX,
    SPHERE,
    PLANE
};

class RigidBody {
public:
    RigidBody();
    ~RigidBody() = default;
    
    void Update(float deltaTime);
    
    void ApplyForce(const glm::vec3& force);
    void ApplyImpulse(const glm::vec3& impulse);
    void ApplyTorque(const glm::vec3& torque);
    void ApplyAngularImpulse(const glm::vec3& angularImpulse);
    
    const glm::vec3& GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    
    const glm::vec3& GetLinearVelocity() const { return m_LinearVelocity; }
    void SetLinearVelocity(const glm::vec3& vel) { m_LinearVelocity = vel; }
    
    const glm::vec3& GetForce() const { return m_Force; }
    void SetForce(const glm::vec3& force) { m_Force = force; }
    
    float GetMass() const { return m_Mass; }
    void SetMass(float mass);
    
    float GetInverseMass() const { return m_InverseMass; }
    
    const glm::vec3& GetGravity() const { return m_Gravity; }
    void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }
    
    BodyType GetBodyType() const { return m_BodyType; }
    void SetBodyType(BodyType type);
    
    float GetFriction() const { return m_Friction; }
    void SetFriction(float friction) { m_Friction = friction; }
    
    float GetRestitution() const { return m_Restitution; }
    void SetRestitution(float restitution) { m_Restitution = restitution; }
    
    bool IsStatic() const { return m_BodyType == BodyType::STATIC; }
    bool IsDynamic() const { return m_BodyType == BodyType::DYNAMIC; }
    
    void ClearForces();
    
    ColliderType GetColliderType() const { return m_ColliderType; }
    void SetColliderType(ColliderType type) { m_ColliderType = type; }
    
    const glm::vec3& GetColliderSize() const { return m_ColliderSize; }
    void SetColliderSize(const glm::vec3& size) { m_ColliderSize = size; }
    
    float GetColliderRadius() const { return m_ColliderRadius; }
    void SetColliderRadius(float radius) { m_ColliderRadius = radius; }
    
private:
    glm::vec3 m_Position;
    glm::vec3 m_LinearVelocity;
    glm::vec3 m_Force;
    
    glm::vec3 m_Rotation;
    glm::vec3 m_AngularVelocity;
    glm::vec3 m_Torque;
    
    float m_Mass;
    float m_InverseMass;
    glm::vec3 m_Gravity;
    
    BodyType m_BodyType;
    float m_Friction;
    float m_Restitution;
    float m_LinearDamping;
    float m_AngularDamping;
    
    ColliderType m_ColliderType;
    glm::vec3 m_ColliderSize;
    float m_ColliderRadius;
};

}
