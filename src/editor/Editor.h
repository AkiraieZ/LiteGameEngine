#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace LGE {

using CreateCubeCallback = std::function<void(const glm::vec3& position)>;

struct EditorEntity {
    std::string Name;
    bool Selected = false;
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
    
    std::vector<EditorEntity> m_Entities;
    int m_SelectedEntityIndex;
    
    CreateCubeCallback m_CreateCubeCallback;
};

}
