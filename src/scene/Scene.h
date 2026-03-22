#pragma once

#include <string>
#include <memory>
#include "Entity.h"
#include "Components.h"
#include "../renderer/Camera.h"
#include "../physics/PhysicsWorld.h"

namespace LGE {

class Scene {
public:
    Scene(const std::string& name = "Untitled");
    ~Scene();
    
    void Init();
    void Update(float deltaTime);
    void Render();
    void Shutdown();
    
    EntityID CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(EntityID entity);
    
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }
    
    Camera& GetCamera() { return m_Camera; }
    PhysicsWorld& GetPhysicsWorld() { return *m_PhysicsWorld; }
    
    Coordinator& GetCoordinator() { return *m_Coordinator; }
    
private:
    std::string m_Name;
    std::unique_ptr<Coordinator> m_Coordinator;
    std::unique_ptr<PhysicsWorld> m_PhysicsWorld;
    Camera m_Camera;
};

}
