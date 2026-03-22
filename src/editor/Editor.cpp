#include "Editor.h"
#include "../core/Logger.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace LGE {

Editor::Editor()
    : m_Initialized(false), m_ShowDemoWindow(false),
      m_ShowHierarchy(true), m_ShowInspector(true),
      m_ShowConsole(true), m_ShowStats(true),
      m_DemoPaused(false), m_SelectedEntityIndex(-1) {
}

Editor::~Editor() {
    Shutdown();
}

void Editor::Init(GLFWwindow* window) {
    if (m_Initialized) return;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    
    AddTestEntity();
    AddTestEntity();
    
    m_Initialized = true;
    LGE_LOG_INFO("Editor initialized");
}

void Editor::Shutdown() {
    if (!m_Initialized) return;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    m_Initialized = false;
    LGE_LOG_INFO("Editor shutdown");
}

void Editor::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Editor::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::Render() {
    RenderMainMenuBar();
    
    if (m_ShowHierarchy) RenderSceneHierarchy();
    if (m_ShowInspector) RenderInspector();
    if (m_ShowConsole) RenderConsole();
    if (m_ShowStats) RenderStats();
    
    if (m_ShowDemoWindow) {
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }
}

void Editor::AddTestEntity() {
    EditorEntity entity;
    entity.Name = "Entity " + std::to_string(m_Entities.size() + 1);
    entity.Selected = false;
    m_Entities.push_back(entity);
}

void Editor::RenderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                LGE_LOG_INFO("Undo clicked!");
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                LGE_LOG_INFO("Redo clicked!");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Preferences")) {
                LGE_LOG_INFO("Preferences clicked!");
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hierarchy", nullptr, &m_ShowHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector);
            ImGui::MenuItem("Console", nullptr, &m_ShowConsole);
            ImGui::MenuItem("Stats", nullptr, &m_ShowStats);
            ImGui::Separator();
            ImGui::MenuItem("Demo Window", nullptr, &m_ShowDemoWindow);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Create Cube")) {
                LGE_LOG_INFO("Create Cube clicked!");
                AddTestEntity();
                if (m_CreateCubeCallback) {
                    m_CreateCubeCallback(glm::vec3(0.0f, 5.0f, 0.0f));
                }
            }
            if (ImGui::MenuItem("Create Sphere")) {
                LGE_LOG_INFO("Create Sphere clicked!");
                AddTestEntity();
            }
            if (ImGui::MenuItem("Create Plane")) {
                LGE_LOG_INFO("Create Plane clicked!");
                AddTestEntity();
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Editor::RenderSceneHierarchy() {
    ImGui::Begin("Hierarchy", &m_ShowHierarchy);
    
    ImGui::Text("Entities:");
    ImGui::Separator();
    
    if (ImGui::Button("Add Entity")) {
        LGE_LOG_INFO("Add Entity button clicked!");
        AddTestEntity();
    }
    
    ImGui::Separator();
    
    for (size_t i = 0; i < m_Entities.size(); i++) {
        bool isSelected = (m_SelectedEntityIndex == (int)i);
        if (ImGui::Selectable(m_Entities[i].Name.c_str(), isSelected)) {
            m_SelectedEntityIndex = (int)i;
            LGE_LOG_INFO("Selected entity: %s", m_Entities[i].Name.c_str());
        }
    }
    
    ImGui::End();
}

void Editor::RenderInspector() {
    ImGui::Begin("Inspector", &m_ShowInspector);
    
    if (m_SelectedEntityIndex >= 0 && m_SelectedEntityIndex < (int)m_Entities.size()) {
        ImGui::Text("Selected: %s", m_Entities[m_SelectedEntityIndex].Name.c_str());
        ImGui::Separator();
        
        static float position[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::DragFloat3("Position", position, 0.1f);
        
        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::DragFloat3("Rotation", rotation, 0.1f);
        
        static float scale[3] = { 1.0f, 1.0f, 1.0f };
        ImGui::DragFloat3("Scale", scale, 0.1f);
    } else {
        ImGui::Text("No entity selected");
    }
    
    ImGui::End();
}

void Editor::RenderConsole() {
    ImGui::Begin("Console", &m_ShowConsole);
    
    ImGui::Text("Console output will appear here");
    ImGui::Separator();
    
    ImGui::TextWrapped("Use the menu bar and Hierarchy panel to interact with the engine.");
    ImGui::TextWrapped("Try clicking 'Add Entity' or using Tools -> Create Cube!");
    
    ImGui::End();
}

void Editor::RenderStats() {
    ImGui::Begin("Stats", &m_ShowStats);
    
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                1000.0f / io.Framerate, io.Framerate);
    
    ImGui::Separator();
    
    if (ImGui::Button(m_DemoPaused ? "Resume" : "Pause")) {
        m_DemoPaused = !m_DemoPaused;
        LGE_LOG_INFO("Demo %s", m_DemoPaused ? "paused" : "resumed");
    }
    
    ImGui::Separator();
    
    ImGui::Text("Entities in scene: %zu", m_Entities.size());
    
    ImGui::End();
}

}
