#pragma once

#include <string>
#include <memory>
#include <atomic>

namespace LGE {

enum class ResourceType {
    UNKNOWN,
    TEXTURE,
    MODEL,
    SHADER,
    MESH,
    MATERIAL,
    SOUND,
    SCRIPT,
    SCENE
};

class Resource {
public:
    Resource(ResourceType type, const std::string& name = "");
    virtual ~Resource() = default;
    
    virtual bool Load(const std::string& filepath) = 0;
    virtual void Unload() = 0;
    virtual bool IsLoaded() const { return m_IsLoaded; }
    
    ResourceType GetType() const { return m_Type; }
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }
    
    const std::string& GetFilePath() const { return m_FilePath; }
    void SetFilePath(const std::string& path) { m_FilePath = path; }
    
    uint32_t GetReferenceCount() const { return m_RefCount; }
    void AddReference() { m_RefCount++; }
    void RemoveReference() { if (m_RefCount > 0) m_RefCount--; }
    
protected:
    ResourceType m_Type;
    std::string m_Name;
    std::string m_FilePath;
    bool m_IsLoaded;
    std::atomic<uint32_t> m_RefCount;
};

}
