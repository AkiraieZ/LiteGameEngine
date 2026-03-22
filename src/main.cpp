#include <iostream>
#include <memory>
#include <vector>
#include "core/Logger.h"
#include "core/Timer.h"
#include "renderer/Window.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"
#include "renderer/Texture.h"
#include "renderer/Camera.h"
#include "input/Input.h"
#include "editor/Editor.h"
#include "scene/Scene.h"
#include "physics/RigidBody.h"
#include "physics/PhysicsWorld.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SceneCube {
    std::shared_ptr<LGE::RigidBody> Body;
    glm::vec3 Color;
};

struct SceneLight {
    glm::vec3 Position;
    glm::vec3 Color;
    float Intensity;
};

std::shared_ptr<LGE::Mesh> CreateCubeMesh() {
    std::vector<LGE::Vertex> vertices = {
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
        { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { -0.5f, 0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } }
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 6, 5, 6, 4, 7,
        8, 9, 10, 10, 11, 8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
    
    return std::make_shared<LGE::Mesh>(vertices, indices);
}

std::shared_ptr<LGE::Mesh> CreatePlaneMesh() {
    std::vector<LGE::Vertex> vertices = {
        { { -50.0f, 0.0f, -50.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 50.0f, 0.0f, -50.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 50.0f, 0.0f, 50.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
        { { -50.0f, 0.0f, 50.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }
    };
    
    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };
    
    return std::make_shared<LGE::Mesh>(vertices, indices);
}

glm::vec3 ScreenToWorld(glm::vec2 screenPos, const LGE::Camera& camera, float depth = 10.0f) {
    float x = (2.0f * screenPos.x) / 1280.0f - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / 720.0f;
    
    glm::vec4 clipCoords(x, y, -1.0f, 1.0f);
    
    glm::mat4 invProj = glm::inverse(camera.GetProjectionMatrix());
    glm::vec4 viewCoords = invProj * clipCoords;
    viewCoords.z = -1.0f;
    viewCoords.w = 0.0f;
    
    glm::mat4 invView = glm::inverse(camera.GetViewMatrix());
    glm::vec3 worldDir = glm::vec3(invView * viewCoords);
    worldDir = glm::normalize(worldDir);
    
    return camera.GetPosition() + worldDir * depth;
}

int main() {
    LGE::Logger::Init();
    LGE_LOG_INFO("LightGameEngine - Starting...");
    
    LGE::Window::WindowProps windowProps;
    windowProps.Title = "LightGameEngine - 基于C++与OpenGL的轻量级游戏引擎";
    windowProps.Width = 1280;
    windowProps.Height = 720;
    windowProps.VSync = true;
    
    auto window = std::make_unique<LGE::Window>(windowProps);
    
    LGE::Input::Init(window->GetNativeWindow());
    
    auto editor = std::make_unique<LGE::Editor>();
    editor->Init(window->GetNativeWindow());
    
    auto shader = std::make_shared<LGE::Shader>("shaders/basic.vert", "shaders/basic.frag");
    auto cubeMesh = CreateCubeMesh();
    auto planeMesh = CreatePlaneMesh();
    
    auto scene = std::make_unique<LGE::Scene>("Demo Scene");
    scene->Init();
    
    auto& camera = scene->GetCamera();
    camera.SetPosition(glm::vec3(0.0f, 3.0f, 8.0f));
    camera.SetAspectRatio((float)windowProps.Width / (float)windowProps.Height);
    
    auto& physicsWorld = scene->GetPhysicsWorld();
    
    std::vector<SceneCube> sceneCubes;
    std::vector<SceneLight> sceneLights;
    
    auto groundBody = std::make_shared<LGE::RigidBody>();
    groundBody->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    groundBody->SetBodyType(LGE::BodyType::STATIC);
    groundBody->SetMass(0.0f);
    groundBody->SetColliderType(LGE::ColliderType::PLANE);
    physicsWorld.AddRigidBody(groundBody);
    
    auto addCubeAtPosition = [&](const glm::vec3& position) {
        auto cubeBody = std::make_shared<LGE::RigidBody>();
        cubeBody->SetPosition(position);
        cubeBody->SetMass(1.0f);
        cubeBody->SetColliderType(LGE::ColliderType::BOX);
        cubeBody->SetColliderSize(glm::vec3(1.0f, 1.0f, 1.0f));
        physicsWorld.AddRigidBody(cubeBody);
        
        SceneCube cube;
        cube.Body = cubeBody;
        cube.Color = glm::vec3(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX
        );
        sceneCubes.push_back(cube);
        
        LGE::EditorEntity entity;
        entity.Name = "Cube " + std::to_string(sceneCubes.size());
        entity.Selected = false;
        entity.IsLight = false;
        entity.Position = position;
        editor->m_Entities.push_back(entity);
        
        LGE_LOG_INFO("Created cube at (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
    };
    
    auto addLightAtPosition = [&](const glm::vec3& position) {
        SceneLight light;
        light.Position = position;
        light.Color = glm::vec3(1.0f, 1.0f, 0.8f);
        light.Intensity = 2.0f;
        sceneLights.push_back(light);
        
        LGE::EditorEntity entity;
        entity.Name = "Light " + std::to_string(sceneLights.size());
        entity.Selected = false;
        entity.IsLight = true;
        entity.Position = position;
        editor->m_Entities.push_back(entity);
        
        LGE_LOG_INFO("Created point light at (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
    };
    
    editor->SetCreateCubeCallback(addCubeAtPosition);
    editor->SetCreateLightCallback(addLightAtPosition);
    
    addCubeAtPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    sceneCubes[0].Color = glm::vec3(0.8f, 0.3f, 0.3f);
    
    addCubeAtPosition(glm::vec3(2.0f, 7.0f, 0.0f));
    sceneCubes[1].Color = glm::vec3(0.3f, 0.3f, 0.8f);
    
    addLightAtPosition(glm::vec3(5.0f, 5.0f, 5.0f));
    
    auto deleteEntityCallback = [&](int index) {
        int lightCount = 0;
        int cubeCount = 0;
        
        for (size_t i = 0; i < editor->m_Entities.size(); i++) {
            if (editor->m_Entities[i].IsLight) {
                if (i == (size_t)index) {
                    sceneLights.erase(sceneLights.begin() + lightCount);
                    LGE_LOG_INFO("Deleted light at index %d", index);
                    return;
                }
                lightCount++;
            } else {
                if (i == (size_t)index) {
                    physicsWorld.RemoveRigidBody(sceneCubes[cubeCount].Body);
                    sceneCubes.erase(sceneCubes.begin() + cubeCount);
                    LGE_LOG_INFO("Deleted cube at index %d", index);
                    return;
                }
                cubeCount++;
            }
        }
    };
    
    editor->SetDeleteEntityCallback(deleteEntityCallback);
    
    auto updateEntityPositionCallback = [&](int index, const glm::vec3& position) {
        int lightCount = 0;
        int cubeCount = 0;
        
        for (size_t i = 0; i < editor->m_Entities.size(); i++) {
            if (editor->m_Entities[i].IsLight) {
                if (i == (size_t)index) {
                    sceneLights[lightCount].Position = position;
                    LGE_LOG_INFO("Updated light position to (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
                    return;
                }
                lightCount++;
            } else {
                if (i == (size_t)index) {
                    sceneCubes[cubeCount].Body->SetPosition(position);
                    LGE_LOG_INFO("Updated cube position to (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
                    return;
                }
                cubeCount++;
            }
        }
    };
    
    editor->SetUpdateEntityPositionCallback(updateEntityPositionCallback);
    
    bool firstMouse = true;
    float lastX = windowProps.Width / 2.0f;
    float lastY = windowProps.Height / 2.0f;
    bool leftMousePressedLastFrame = false;
    
    window->SetEventCallback([&](int type, int a, int b, int c) {
        if (type == 1) {
            LGE::Input::OnKey(a, c);
        } else if (type == 2) {
            LGE::Input::OnMouseButton(a, b);
        } else if (type == 3) {
            float xpos = (float)a;
            float ypos = (float)b;
            LGE::Input::OnMouseMove(xpos, ypos);
            
            if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }
            
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;
            
            if (LGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
                camera.ProcessMouseMovement(xoffset, yoffset);
            }
        }
    });
    
    LGE::Timer frameTimer;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    LGE_LOG_INFO("Engine initialized successfully!");
    
    while (!window->ShouldClose()) {
        float currentFrame = static_cast<float>(frameTimer.Elapsed());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        if (LGE::Input::IsKeyPressed(GLFW_KEY_W)) camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
        if (LGE::Input::IsKeyPressed(GLFW_KEY_S)) camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
        if (LGE::Input::IsKeyPressed(GLFW_KEY_A)) camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
        if (LGE::Input::IsKeyPressed(GLFW_KEY_D)) camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
        if (LGE::Input::IsKeyPressed(GLFW_KEY_SPACE)) camera.ProcessKeyboard(GLFW_KEY_SPACE, deltaTime);
        if (LGE::Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) camera.ProcessKeyboard(GLFW_KEY_LEFT_SHIFT, deltaTime);
        
        bool leftMousePressed = LGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        if (leftMousePressed && !leftMousePressedLastFrame) {
            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse) {
                double mouseX, mouseY;
                glfwGetCursorPos(window->GetNativeWindow(), &mouseX, &mouseY);
                glm::vec3 worldPos = ScreenToWorld(glm::vec2(mouseX, mouseY), camera, 10.0f);
                worldPos.y = 5.0f;
                addCubeAtPosition(worldPos);
            }
        }
        leftMousePressedLastFrame = leftMousePressed;
        
        scene->Update(deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        shader->Bind();
        shader->SetUniformMat4("u_View", camera.GetViewMatrix());
        shader->SetUniformMat4("u_Projection", camera.GetProjectionMatrix());
        shader->SetUniformVec3("u_ViewPos", camera.GetPosition());
        
        int lightCount = std::min((int)sceneLights.size(), 4);
        shader->SetUniform1i("u_LightCount", lightCount);
        
        for (int i = 0; i < lightCount; i++) {
            shader->SetUniformVec3("u_LightPositions[" + std::to_string(i) + "]", sceneLights[i].Position);
            shader->SetUniformVec3("u_LightColors[" + std::to_string(i) + "]", sceneLights[i].Color);
            shader->SetUniform1f("u_LightIntensities[" + std::to_string(i) + "]", sceneLights[i].Intensity);
        }
        
        glm::mat4 groundTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
        shader->SetUniformMat4("u_Model", groundTransform);
        shader->SetUniformVec3("u_Albedo", glm::vec3(0.3f, 0.5f, 0.3f));
        planeMesh->Render();
        
        for (const auto& cube : sceneCubes) {
            glm::mat4 cubeTransform = glm::translate(glm::mat4(1.0f), cube.Body->GetPosition());
            shader->SetUniformMat4("u_Model", cubeTransform);
            shader->SetUniformVec3("u_Albedo", cube.Color);
            cubeMesh->Render();
        }
        
        shader->Unbind();
        
        editor->BeginFrame();
        editor->Render();
        editor->EndFrame();
        
        window->OnUpdate();
    }
    
    scene->Shutdown();
    editor->Shutdown();
    LGE::Input::Shutdown();
    LGE::Logger::Shutdown();
    
    return 0;
}
