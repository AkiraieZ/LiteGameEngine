#include "RigidBody.h"

namespace LGE {

RigidBody::RigidBody()
    : m_Position(0.0f), m_LinearVelocity(0.0f), m_Force(0.0f),
      m_Rotation(0.0f), m_AngularVelocity(0.0f), m_Torque(0.0f),
      m_Mass(1.0f), m_InverseMass(1.0f), m_Gravity(0.0f, -9.8f, 0.0f),
      m_BodyType(BodyType::DYNAMIC), m_Friction(0.3f), m_Restitution(0.5f),
      m_LinearDamping(0.99f), m_AngularDamping(0.99f),
      m_ColliderType(ColliderType::BOX), m_ColliderSize(1.0f), m_ColliderRadius(1.0f) {
}

void RigidBody::SetBodyType(BodyType type) {
    m_BodyType = type;
    if (m_BodyType == BodyType::STATIC) {
        m_InverseMass = 0.0f;
    } else {
        m_InverseMass = (m_Mass > 0.0f) ? 1.0f / m_Mass : 0.0f;
    }
}

void RigidBody::SetMass(float mass) {
    m_Mass = mass;
    if (m_BodyType == BodyType::STATIC) {
        m_InverseMass = 0.0f;
    } else {
        m_InverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
    }
}

void RigidBody::ApplyForce(const glm::vec3& force) {
    m_Force += force;
}

void RigidBody::ApplyImpulse(const glm::vec3& impulse) {
    m_LinearVelocity += impulse * m_InverseMass;
}

void RigidBody::ApplyTorque(const glm::vec3& torque) {
    m_Torque += torque;
}

void RigidBody::ApplyAngularImpulse(const glm::vec3& angularImpulse) {
    m_AngularVelocity += angularImpulse;
}

void RigidBody::ClearForces() {
    m_Force = glm::vec3(0.0f);
    m_Torque = glm::vec3(0.0f);
}

void RigidBody::Update(float deltaTime) {
    if (m_BodyType == BodyType::STATIC) {
        ClearForces();
        return;
    }
    
    m_Force += m_Gravity * m_Mass;
    
    glm::vec3 acceleration = m_Force * m_InverseMass;
    m_LinearVelocity += acceleration * deltaTime;
    m_LinearVelocity *= m_LinearDamping;
    
    m_Position += m_LinearVelocity * deltaTime;
    
    m_AngularVelocity += m_Torque * deltaTime;
    m_AngularVelocity *= m_AngularDamping;
    m_Rotation += m_AngularVelocity * deltaTime;
    
    ClearForces();
}

}
