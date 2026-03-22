#include "Entity.h"

namespace LGE {

EntityManager::EntityManager()
    : m_LivingEntityCount(0) {
    for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
        m_AvailableEntities.push(entity);
    }
}

EntityID EntityManager::CreateEntity() {
    EntityID id = m_AvailableEntities.front();
    m_AvailableEntities.pop();
    m_LivingEntityCount++;
    return id;
}

void EntityManager::DestroyEntity(EntityID entity) {
    m_Signatures[entity].reset();
    m_AvailableEntities.push(entity);
    m_LivingEntityCount--;
}

void EntityManager::SetSignature(EntityID entity, Signature signature) {
    m_Signatures[entity] = signature;
}

Signature EntityManager::GetSignature(EntityID entity) {
    return m_Signatures[entity];
}

void SystemManager::EntityDestroyed(EntityID entity) {
    for (auto const& pair : m_Systems) {
        auto const& system = pair.second;
        system->m_Entities.erase(entity);
    }
}

void SystemManager::EntitySignatureChanged(EntityID entity, Signature entitySignature) {
    for (auto const& pair : m_Systems) {
        auto const& type = pair.first;
        auto const& system = pair.second;
        auto const& systemSignature = m_Signatures[type];
        
        if ((entitySignature & systemSignature) == systemSignature) {
            system->m_Entities.insert(entity);
        } else {
            system->m_Entities.erase(entity);
        }
    }
}

void Coordinator::Init() {
    m_ComponentManager = std::make_unique<ComponentManager>();
    m_EntityManager = std::make_unique<EntityManager>();
    m_SystemManager = std::make_unique<SystemManager>();
}

EntityID Coordinator::CreateEntity() {
    return m_EntityManager->CreateEntity();
}

void Coordinator::DestroyEntity(EntityID entity) {
    m_EntityManager->DestroyEntity(entity);
    m_ComponentManager->EntityDestroyed(entity);
    m_SystemManager->EntityDestroyed(entity);
}

}
