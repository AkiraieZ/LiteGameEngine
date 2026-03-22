#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace LGE {

using CreateCubeCallback = std::function<void(const glm::vec3& position)>;
using CreateLightCallback = std::function<void(const glm::vec3& position)>;
using DeleteEntityCallback = std::function<void(int index)>;
using UpdateEntityPositionCallback = std::function<void(int index, const glm::vec3& position)>;

struct EditorEntity {
    std::string Name;
    bool Selected = false;
    bool IsLight = false;
    glm::vec3 Position = glm::vec3(0.0f);
};

class Editor {
public:
    Editor();
    ~Editor();
    
    void Init(GLFWwindow* window);
    void Shutdown();
    
    void BeginFrame();
    void EndFrame();
    
    void Render();
    
    bool IsInitialized() const { return m_Initialized; }
    
    bool IsDemoPaused() const { return m_DemoPaused; }
    
    void SetCreateCubeCallback(CreateCubeCallback callback) { m_CreateCubeCallback = callback; }
    void SetCreateLightCallback(CreateLightCallback callback) { m_CreateLightCallback = callback; }
    void SetDeleteEntityCallback(DeleteEntityCallback callback) { m_DeleteEntityCallback = callback; }
    void SetUpdateEntityPositionCallback(UpdateEntityPositionCallback callback) { m_UpdateEntityPositionCallback = callback; }
    
    std::vector<EditorEntity> m_Entities;
    
private:
    void RenderMainMenuBar();
    void RenderSceneHierarchy();
    void RenderInspector();
    void RenderConsole();
    void RenderStats();
    
    void AddTestEntity();
    
    bool m_Initialized;
    bool m_ShowDemoWindow;
    bool m_ShowHierarchy;
    bool m_ShowInspector;
    bool m_ShowConsole;
    bool m_ShowStats;
    bool m_DemoPaused;
    
    int m_SelectedEntityIndex;
    
    CreateCubeCallback m_CreateCubeCallback;
    CreateLightCallback m_CreateLightCallback;
    DeleteEntityCallback m_DeleteEntityCallback;
    UpdateEntityPositionCallback m_UpdateEntityPositionCallback;
};

}
