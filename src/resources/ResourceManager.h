#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <future>
#include "Resource.h"
#include "../core/Logger.h"

namespace LGE {

class ResourceManager {
public:
    static ResourceManager& GetInstance();
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    void Init();
    void Shutdown();
    void Update();
    
    template<typename T>
    std::shared_ptr<T> LoadResource(const std::string& name, const std::string& filepath);
    
    template<typename T>
    std::shared_ptr<T> GetResource(const std::string& name);
    
    void UnloadResource(const std::string& name);
    void UnloadAllResources();
    
    bool ResourceExists(const std::string& name) const;
    
    template<typename T>
    void RegisterLoader(std::function<std::shared_ptr<T>(const std::string&)> loader);
    
    size_t GetResourceCount() const;
    void CollectGarbage();
    
    void EnableAsyncLoading(bool enable) { m_AsyncLoading = enable; }
    bool IsAsyncLoadingEnabled() const { return m_AsyncLoading; }
    
private:
    ResourceManager();
    ~ResourceManager();
    
    void ProcessLoadQueue();
    void ProcessUnloadQueue();
    
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_Resources;
    std::unordered_map<std::string, std::function<std::shared_ptr<Resource>(const std::string&)>> m_Loaders;
    
    std::queue<std::pair<std::string, std::string>> m_LoadQueue;
    std::queue<std::string> m_UnloadQueue;
    
    std::mutex m_ResourceMutex;
    std::mutex m_QueueMutex;
    
    std::thread m_LoadThread;
    std::atomic<bool> m_Running;
    std::atomic<bool> m_AsyncLoading;
    
    static ResourceManager* s_Instance;
};

template<typename T>
std::shared_ptr<T> ResourceManager::LoadResource(const std::string& name, const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_ResourceMutex);
    
    auto it = m_Resources.find(name);
    if (it != m_Resources.end()) {
        it->second->AddReference();
        return std::dynamic_pointer_cast<T>(it->second);
    }
    
    auto loaderIt = m_Loaders.find(typeid(T).name());
    if (loaderIt != m_Loaders.end()) {
        auto resource = std::dynamic_pointer_cast<T>(loaderIt->second(filepath));
        if (resource) {
            resource->SetName(name);
            resource->SetFilePath(filepath);
            resource->AddReference();
            m_Resources[name] = resource;
            LGE_LOG_INFO("Loaded resource: %s", name.c_str());
            return resource;
        }
    }
    
    LGE_LOG_ERROR("Failed to load resource: %s", name.c_str());
    return nullptr;
}

template<typename T>
std::shared_ptr<T> ResourceManager::GetResource(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_ResourceMutex);
    
    auto it = m_Resources.find(name);
    if (it != m_Resources.end()) {
        return std::dynamic_pointer_cast<T>(it->second);
    }
    
    return nullptr;
}

template<typename T>
void ResourceManager::RegisterLoader(std::function<std::shared_ptr<T>(const std::string&)> loader) {
    std::lock_guard<std::mutex> lock(m_ResourceMutex);
    m_Loaders[typeid(T).name()] = [loader](const std::string& path) -> std::shared_ptr<Resource> {
        return loader(path);
    };
    LGE_LOG_INFO("Registered loader for type: %s", typeid(T).name());
}

}
