#include "ResourceManager.h"
#include <algorithm>

namespace LGE {

ResourceManager* ResourceManager::s_Instance = nullptr;

ResourceManager& ResourceManager::GetInstance() {
    if (!s_Instance) {
        s_Instance = new ResourceManager();
    }
    return *s_Instance;
}

ResourceManager::ResourceManager()
    : m_Running(false), m_AsyncLoading(false) {
}

ResourceManager::~ResourceManager() {
    Shutdown();
}

void ResourceManager::Init() {
    if (m_Running) return;
    
    m_Running = true;
    
    if (m_AsyncLoading) {
        m_LoadThread = std::thread([this]() {
            while (m_Running) {
                ProcessLoadQueue();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    LGE_LOG_INFO("ResourceManager initialized");
}

void ResourceManager::Shutdown() {
    if (!m_Running) return;
    
    m_Running = false;
    
    if (m_LoadThread.joinable()) {
        m_LoadThread.join();
    }
    
    UnloadAllResources();
    LGE_LOG_INFO("ResourceManager shutdown");
}

void ResourceManager::Update() {
    if (!m_AsyncLoading) {
        ProcessLoadQueue();
    }
    ProcessUnloadQueue();
}

void ResourceManager::ProcessLoadQueue() {
    std::lock_guard<std::mutex> lock(m_QueueMutex);
    
    while (!m_LoadQueue.empty()) {
        auto [name, path] = m_LoadQueue.front();
        m_LoadQueue.pop();
    }
}

void ResourceManager::ProcessUnloadQueue() {
    std::lock_guard<std::mutex> lock(m_QueueMutex);
    
    while (!m_UnloadQueue.empty()) {
        std::string name = m_UnloadQueue.front();
        m_UnloadQueue.pop();
        
        auto it = m_Resources.find(name);
        if (it != m_Resources.end()) {
            it->second->Unload();
            m_Resources.erase(it);
            LGE_LOG_INFO("Unloaded resource: %s", name.c_str());
        }
    }
}

void ResourceManager::UnloadResource(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_ResourceMutex);
    
    auto it = m_Resources.find(name);
    if (it != m_Resources.end()) {
        it->second->RemoveReference();
        if (it->second->GetReferenceCount() == 0) {
            std::lock_guard<std::mutex> queueLock(m_QueueMutex);
            m_UnloadQueue.push(name);
        }
    }
}

void ResourceManager::UnloadAllResources() {
    std::lock_guard<std::mutex> lock(m_ResourceMutex);
    
    for (auto& pair : m_Resources) {
        pair.second->Unload();
    }
    m_Resources.clear();
    LGE_LOG_INFO("Unloaded all resources");
}

bool ResourceManager::ResourceExists(const std::string& name) const {
    return m_Resources.find(name) != m_Resources.end();
}

size_t ResourceManager::GetResourceCount() const {
    return m_Resources.size();
}

void ResourceManager::CollectGarbage() {
    std::lock_guard<std::mutex> lock(m_ResourceMutex);
    
    std::vector<std::string> toUnload;
    for (auto& pair : m_Resources) {
        if (pair.second->GetReferenceCount() == 0) {
            toUnload.push_back(pair.first);
        }
    }
    
    for (const auto& name : toUnload) {
        UnloadResource(name);
    }
    
    if (!toUnload.empty()) {
        LGE_LOG_INFO("Collected %zu unused resources", toUnload.size());
    }
}

}
