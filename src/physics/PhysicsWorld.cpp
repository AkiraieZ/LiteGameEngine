#include "PhysicsWorld.h"
#include "../core/Logger.h"
#include <algorithm>

namespace LGE {

PhysicsWorld::PhysicsWorld()
    : m_Gravity(0.0f, -9.8f, 0.0f), m_FixedTimestep(1.0f / 60.0f),
      m_Accumulator(0.0f), m_SolverIterations(4) {
}

PhysicsWorld::~PhysicsWorld() {
}

void PhysicsWorld::AddRigidBody(std::shared_ptr<RigidBody> body) {
    m_RigidBodies.push_back(body);
}

void PhysicsWorld::RemoveRigidBody(std::shared_ptr<RigidBody> body) {
    auto it = std::find(m_RigidBodies.begin(), m_RigidBodies.end(), body);
    if (it != m_RigidBodies.end()) {
        m_RigidBodies.erase(it);
    }
}

void PhysicsWorld::Update(float deltaTime) {
    m_Accumulator += deltaTime;
    
    while (m_Accumulator >= m_FixedTimestep) {
        for (auto& body : m_RigidBodies) {
            body->Update(m_FixedTimestep);
        }
        
        CheckCollisions();
        ResolveCollisions();
        
        m_Accumulator -= m_FixedTimestep;
    }
}

void PhysicsWorld::CheckCollisions() {
    m_Collisions.clear();
    
    for (size_t i = 0; i < m_RigidBodies.size(); i++) {
        for (size_t j = i + 1; j < m_RigidBodies.size(); j++) {
            auto bodyA = m_RigidBodies[i];
            auto bodyB = m_RigidBodies[j];
            
            if (bodyA->IsStatic() && bodyB->IsStatic()) {
                continue;
            }
            
            CollisionInfo info;
            if (CheckCollision(bodyA, bodyB, info)) {
                CollisionPair pair;
                pair.BodyA = bodyA;
                pair.BodyB = bodyB;
                pair.Info = info;
                m_Collisions.push_back(pair);
            }
        }
    }
}

bool PhysicsWorld::CheckCollision(std::shared_ptr<RigidBody> bodyA, 
                                    std::shared_ptr<RigidBody> bodyB,
                                    CollisionInfo& info) {
    auto typeA = bodyA->GetColliderType();
    auto typeB = bodyB->GetColliderType();
    
    if (typeA == ColliderType::PLANE || typeB == ColliderType::PLANE) {
        return CheckPlaneCollision(bodyA, bodyB, info);
    } else {
        return CheckBoxCollision(bodyA, bodyB, info);
    }
}

bool PhysicsWorld::CheckPlaneCollision(std::shared_ptr<RigidBody> bodyA,
                                         std::shared_ptr<RigidBody> bodyB,
                                         CollisionInfo& info) {
    auto planeBody = (bodyA->GetColliderType() == ColliderType::PLANE) ? bodyA : bodyB;
    auto otherBody = (bodyA->GetColliderType() == ColliderType::PLANE) ? bodyB : bodyA;
    
    info.BodyA = planeBody;
    info.BodyB = otherBody;
    
    float planeY = planeBody->GetPosition().y;
    float otherY = otherBody->GetPosition().y;
    float halfHeight = otherBody->GetColliderSize().y * 0.5f;
    
    float otherBottomY = otherY - halfHeight;
    
    if (otherBottomY <= planeY) {
        info.Hit = true;
        info.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        info.Distance = planeY - otherBottomY;
        info.Point = glm::vec3(otherBody->GetPosition().x, planeY, otherBody->GetPosition().z);
        
        return true;
    }
    
    return false;
}

bool PhysicsWorld::CheckBoxCollision(std::shared_ptr<RigidBody> bodyA,
                                       std::shared_ptr<RigidBody> bodyB,
                                       CollisionInfo& info) {
    info.BodyA = bodyA;
    info.BodyB = bodyB;
    
    auto posA = bodyA->GetPosition();
    auto posB = bodyB->GetPosition();
    auto sizeA = bodyA->GetColliderSize() * 0.5f;
    auto sizeB = bodyB->GetColliderSize() * 0.5f;
    
    glm::vec3 diff = posB - posA;
    glm::vec3 minDist = sizeA + sizeB;
    
    float overlapX = minDist.x - std::abs(diff.x);
    float overlapY = minDist.y - std::abs(diff.y);
    float overlapZ = minDist.z - std::abs(diff.z);
    
    if (overlapX > 0 && overlapY > 0 && overlapZ > 0) {
        info.Hit = true;
        
        if (overlapY < overlapX && overlapY < overlapZ) {
            info.Normal = glm::vec3(0.0f, diff.y > 0 ? -1.0f : 1.0f, 0.0f);
            info.Distance = overlapY;
        } else if (overlapX < overlapZ) {
            info.Normal = glm::vec3(diff.x > 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
            info.Distance = overlapX;
        } else {
            info.Normal = glm::vec3(0.0f, 0.0f, diff.z > 0 ? -1.0f : 1.0f);
            info.Distance = overlapZ;
        }
        
        info.Point = (posA + posB) * 0.5f;
        return true;
    }
    
    return false;
}

void PhysicsWorld::ResolveCollisions() {
    for (auto& collision : m_Collisions) {
        auto& info = collision.Info;
        
        if (!info.Hit) continue;
        
        auto bodyA = info.BodyA;
        auto bodyB = info.BodyB;
        
        if (!bodyA || !bodyB) continue;
        
        auto planeBody = (bodyA->GetColliderType() == ColliderType::PLANE) ? bodyA : bodyB;
        auto otherBody = (bodyA->GetColliderType() == ColliderType::PLANE) ? bodyB : bodyA;
        
        if (planeBody->GetColliderType() == ColliderType::PLANE && !otherBody->IsStatic()) {
            float planeY = planeBody->GetPosition().y;
            float halfHeight = otherBody->GetColliderSize().y * 0.5f;
            
            glm::vec3 pos = otherBody->GetPosition();
            pos.y = planeY + halfHeight;
            otherBody->SetPosition(pos);
            
            glm::vec3 vel = otherBody->GetLinearVelocity();
            vel.y = 0.0f;
            otherBody->SetLinearVelocity(vel);
        }
    }
}

}
