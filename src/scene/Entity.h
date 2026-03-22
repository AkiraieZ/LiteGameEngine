#pragma once

#include <cstdint>
#include <bitset>
#include <array>
#include <queue>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <set>

namespace LGE {

using EntityID = uint32_t;
using ComponentID = uint8_t;
constexpr EntityID MAX_ENTITIES = 5000;
constexpr ComponentID MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

class EntityManager {
public:
    EntityManager();
    
    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    void SetSignature(EntityID entity, Signature signature);
    Signature GetSignature(EntityID entity);
    
private:
    std::queue<EntityID> m_AvailableEntities;
    std::array<Signature, MAX_ENTITIES> m_Signatures;
    uint32_t m_LivingEntityCount;
};

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(EntityID entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
    void InsertData(EntityID entity, T component);
    void RemoveData(EntityID entity);
    T& GetData(EntityID entity);
    bool HasData(EntityID entity);
    void EntityDestroyed(EntityID entity) override;
    
private:
    std::array<T, MAX_ENTITIES> m_ComponentArray;
    std::unordered_map<EntityID, size_t> m_EntityToIndexMap;
    std::unordered_map<size_t, EntityID> m_IndexToEntityMap;
    size_t m_Size;
};

template<typename T>
void ComponentArray<T>::InsertData(EntityID entity, T component) {
    size_t newIndex = m_Size;
    m_EntityToIndexMap[entity] = newIndex;
    m_IndexToEntityMap[newIndex] = entity;
    m_ComponentArray[newIndex] = component;
    m_Size++;
}

template<typename T>
void ComponentArray<T>::RemoveData(EntityID entity) {
    size_t removedEntityIndex = m_EntityToIndexMap[entity];
    size_t lastElementIndex = m_Size - 1;
    EntityID lastEntity = m_IndexToEntityMap[lastElementIndex];
    
    m_ComponentArray[removedEntityIndex] = m_ComponentArray[lastElementIndex];
    m_EntityToIndexMap[lastEntity] = removedEntityIndex;
    m_IndexToEntityMap[removedEntityIndex] = lastEntity;
    
    m_EntityToIndexMap.erase(entity);
    m_IndexToEntityMap.erase(lastElementIndex);
    m_Size--;
}

template<typename T>
T& ComponentArray<T>::GetData(EntityID entity) {
    return m_ComponentArray[m_EntityToIndexMap[entity]];
}

template<typename T>
bool ComponentArray<T>::HasData(EntityID entity) {
    return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
}

template<typename T>
void ComponentArray<T>::EntityDestroyed(EntityID entity) {
    if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
        RemoveData(entity);
    }
}

class ComponentManager {
public:
    template<typename T>
    void RegisterComponent();
    
    template<typename T>
    ComponentID GetComponentID();
    
    template<typename T>
    void AddComponent(EntityID entity, T component);
    
    template<typename T>
    void RemoveComponent(EntityID entity);
    
    template<typename T>
    T& GetComponent(EntityID entity);
    
    template<typename T>
    bool HasComponent(EntityID entity);
    
    void EntityDestroyed(EntityID entity) {
        for (auto const& pair : m_ComponentArrays) {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }
    
private:
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        std::type_index type = typeid(T);
        return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[type]);
    }
    
    std::unordered_map<std::type_index, ComponentID> m_ComponentTypes;
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_ComponentArrays;
    ComponentID m_NextComponentType;
};

template<typename T>
void ComponentManager::RegisterComponent() {
    std::type_index type = typeid(T);
    m_ComponentTypes[type] = m_NextComponentType;
    m_ComponentArrays[type] = std::make_shared<ComponentArray<T>>();
    m_NextComponentType++;
}

template<typename T>
ComponentID ComponentManager::GetComponentID() {
    return m_ComponentTypes[typeid(T)];
}

template<typename T>
void ComponentManager::AddComponent(EntityID entity, T component) {
    GetComponentArray<T>()->InsertData(entity, component);
}

template<typename T>
void ComponentManager::RemoveComponent(EntityID entity) {
    GetComponentArray<T>()->RemoveData(entity);
}

template<typename T>
T& ComponentManager::GetComponent(EntityID entity) {
    return GetComponentArray<T>()->GetData(entity);
}

template<typename T>
bool ComponentManager::HasComponent(EntityID entity) {
    return GetComponentArray<T>()->HasData(entity);
}

class System {
public:
    std::set<EntityID> m_Entities;
};

class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem();
    
    template<typename T>
    void SetSignature(Signature signature);
    
    void EntityDestroyed(EntityID entity);
    void EntitySignatureChanged(EntityID entity, Signature entitySignature);
    
private:
    std::unordered_map<std::type_index, Signature> m_Signatures;
    std::unordered_map<std::type_index, std::shared_ptr<System>> m_Systems;
};

template<typename T>
std::shared_ptr<T> SystemManager::RegisterSystem() {
    std::type_index type = typeid(T);
    auto system = std::make_shared<T>();
    m_Systems[type] = system;
    return system;
}

template<typename T>
void SystemManager::SetSignature(Signature signature) {
    std::type_index type = typeid(T);
    m_Signatures[type] = signature;
}

class Coordinator {
public:
    void Init();
    
    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    
    template<typename T>
    void RegisterComponent() {
        m_ComponentManager->RegisterComponent<T>();
    }
    
    template<typename T>
    void AddComponent(EntityID entity, T component) {
        m_ComponentManager->AddComponent<T>(entity, component);
        
        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentID<T>(), true);
        m_EntityManager->SetSignature(entity, signature);
        
        m_SystemManager->EntitySignatureChanged(entity, signature);
    }
    
    template<typename T>
    void RemoveComponent(EntityID entity) {
        m_ComponentManager->RemoveComponent<T>(entity);
        
        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentID<T>(), false);
        m_EntityManager->SetSignature(entity, signature);
        
        m_SystemManager->EntitySignatureChanged(entity, signature);
    }
    
    template<typename T>
    T& GetComponent(EntityID entity) {
        return m_ComponentManager->GetComponent<T>(entity);
    }
    
    template<typename T>
    bool HasComponent(EntityID entity) {
        return m_ComponentManager->HasComponent<T>(entity);
    }
    
    template<typename T>
    ComponentID GetComponentID() {
        return m_ComponentManager->GetComponentID<T>();
    }
    
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return m_SystemManager->RegisterSystem<T>();
    }
    
    template<typename T>
    void SetSystemSignature(Signature signature) {
        m_SystemManager->SetSignature<T>(signature);
    }
    
private:
    std::unique_ptr<ComponentManager> m_ComponentManager;
    std::unique_ptr<EntityManager> m_EntityManager;
    std::unique_ptr<SystemManager> m_SystemManager;
};

}
