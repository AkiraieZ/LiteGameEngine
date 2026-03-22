#pragma once

#include <vector>
#include <memory>
#include "RigidBody.h"
#include "Collision.h"

namespace LGE {

struct CollisionInfo {
    bool Hit = false;
    glm::vec3 Normal = glm::vec3(0.0f);
    float Distance = 0.0f;
    glm::vec3 Point = glm::vec3(0.0f);
    std::shared_ptr<RigidBody> BodyA;
    std::shared_ptr<RigidBody> BodyB;
};

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();
    
    void Update(float deltaTime);
    
    void AddRigidBody(std::shared_ptr<RigidBody> body);
    void RemoveRigidBody(std::shared_ptr<RigidBody> body);
    
    void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }
    const glm::vec3& GetGravity() const { return m_Gravity; }
    
    void SetFixedTimestep(float timestep) { m_FixedTimestep = timestep; }
    float GetFixedTimestep() const { return m_FixedTimestep; }
    
    void SetSolverIterations(int iterations) { m_SolverIterations = iterations; }
    int GetSolverIterations() const { return m_SolverIterations; }
    
    void CheckCollisions();
    void ResolveCollisions();
    
    struct CollisionPair {
        std::shared_ptr<RigidBody> BodyA;
        std::shared_ptr<RigidBody> BodyB;
        CollisionInfo Info;
    };
    
    const std::vector<CollisionPair>& GetCollisions() const { return m_Collisions; }
    
private:
    bool CheckCollision(std::shared_ptr<RigidBody> bodyA, 
                        std::shared_ptr<RigidBody> bodyB,
                        CollisionInfo& info);
                        
    bool CheckPlaneCollision(std::shared_ptr<RigidBody> bodyA,
                              std::shared_ptr<RigidBody> bodyB,
                              CollisionInfo& info);
                              
    bool CheckBoxCollision(std::shared_ptr<RigidBody> bodyA,
                            std::shared_ptr<RigidBody> bodyB,
                            CollisionInfo& info);
    
    std::vector<std::shared_ptr<RigidBody>> m_RigidBodies;
    std::vector<CollisionPair> m_Collisions;
    glm::vec3 m_Gravity;
    float m_FixedTimestep;
    float m_Accumulator;
    int m_SolverIterations;
};

}
