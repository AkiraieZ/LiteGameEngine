#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <memory>
#include "../renderer/Mesh.h"
#include "../renderer/Shader.h"
#include "../renderer/Texture.h"
#include "../physics/RigidBody.h"

namespace LGE {

struct TransformComponent {
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;
    
    TransformComponent()
        : Position(0.0f), Rotation(0.0f), Scale(1.0f) {}
    
    TransformComponent(const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0.0f), 
                       const glm::vec3& scale = glm::vec3(1.0f))
        : Position(pos), Rotation(rot), Scale(scale) {}
    
    glm::mat4 GetTransform() const {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position);
        transform = glm::rotate(transform, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, Scale);
        return transform;
    }
};

struct TagComponent {
    std::string Tag;
    
    TagComponent() = default;
    TagComponent(const std::string& tag) : Tag(tag) {}
};

struct MeshComponent {
    std::shared_ptr<Mesh> MeshPtr;
    
    MeshComponent() = default;
    MeshComponent(std::shared_ptr<Mesh> mesh) : MeshPtr(mesh) {}
};

struct ShaderComponent {
    std::shared_ptr<Shader> ShaderPtr;
    
    ShaderComponent() = default;
    ShaderComponent(std::shared_ptr<Shader> shader) : ShaderPtr(shader) {}
};

struct MaterialComponent {
    glm::vec3 Albedo;
    float Metallic;
    float Roughness;
    float AO;
    std::shared_ptr<Texture> AlbedoTexture;
    std::shared_ptr<Texture> NormalTexture;
    std::shared_ptr<Texture> MetallicTexture;
    std::shared_ptr<Texture> RoughnessTexture;
    
    MaterialComponent()
        : Albedo(1.0f), Metallic(0.0f), Roughness(0.5f), AO(1.0f) {}
};

struct LightComponent {
    enum class LightType {
        Directional,
        Point,
        Spot
    };
    
    LightType Type;
    glm::vec3 Color;
    float Intensity;
    float Range;
    float CutOff;
    float OuterCutOff;
    bool CastShadows;
    
    LightComponent()
        : Type(LightType::Point), Color(1.0f), Intensity(1.0f), 
          Range(10.0f), CutOff(12.5f), OuterCutOff(17.5f), CastShadows(false) {}
};

struct CameraComponent {
    bool Primary;
    float Fov;
    float Near;
    float Far;
    
    CameraComponent()
        : Primary(true), Fov(45.0f), Near(0.1f), Far(1000.0f) {}
};

struct RigidBodyComponent {
    std::shared_ptr<RigidBody> RigidBodyPtr;
    
    RigidBodyComponent() = default;
    RigidBodyComponent(std::shared_ptr<RigidBody> body) : RigidBodyPtr(body) {}
};

struct ColliderComponent {
    enum class ColliderType {
        Box,
        Sphere,
        Capsule,
        Mesh
    };
    
    ColliderType Type;
    glm::vec3 Size;
    float Radius;
    float Height;
    bool IsTrigger;
    
    ColliderComponent()
        : Type(ColliderType::Box), Size(1.0f), Radius(0.5f), Height(1.0f), IsTrigger(false) {}
};

struct ScriptComponent {
    std::string ScriptName;
    bool Enabled;
    
    ScriptComponent() : Enabled(true) {}
    ScriptComponent(const std::string& name) : ScriptName(name), Enabled(true) {}
};

}
