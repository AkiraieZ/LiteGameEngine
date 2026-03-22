#include "Scene.h"
#include "../core/Logger.h"

namespace LGE {

Scene::Scene(const std::string& name)
    : m_Name(name), m_Coordinator(std::make_unique<Coordinator>()),
      m_PhysicsWorld(std::make_unique<PhysicsWorld>()) {
}

Scene::~Scene() {
    Shutdown();
}

void Scene::Init() {
    m_Coordinator->Init();
    
    m_Coordinator->RegisterComponent<TransformComponent>();
    m_Coordinator->RegisterComponent<TagComponent>();
    m_Coordinator->RegisterComponent<MeshComponent>();
    m_Coordinator->RegisterComponent<ShaderComponent>();
    m_Coordinator->RegisterComponent<MaterialComponent>();
    m_Coordinator->RegisterComponent<LightComponent>();
    m_Coordinator->RegisterComponent<CameraComponent>();
    m_Coordinator->RegisterComponent<RigidBodyComponent>();
    m_Coordinator->RegisterComponent<ColliderComponent>();
    m_Coordinator->RegisterComponent<ScriptComponent>();
    
    LGE_LOG_INFO("Scene '%s' initialized", m_Name.c_str());
}

void Scene::Update(float deltaTime) {
    m_PhysicsWorld->Update(deltaTime);
}

void Scene::Render() {
}

void Scene::Shutdown() {
    LGE_LOG_INFO("Scene '%s' shutdown", m_Name.c_str());
}

EntityID Scene::CreateEntity(const std::string& name) {
    EntityID entity = m_Coordinator->CreateEntity();
    m_Coordinator->AddComponent<TagComponent>(entity, TagComponent(name));
    m_Coordinator->AddComponent<TransformComponent>(entity, TransformComponent());
    return entity;
}

void Scene::DestroyEntity(EntityID entity) {
    m_Coordinator->DestroyEntity(entity);
}

}
