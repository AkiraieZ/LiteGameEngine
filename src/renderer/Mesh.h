#pragma once

#include "Vertex.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace LGE {

class Texture;

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();
    
    void Render();
    
    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    
    void SetTransform(const glm::mat4& transform) { m_Transform = transform; }
    const glm::mat4& GetTransform() const { return m_Transform; }
    
    void AddTexture(std::shared_ptr<Texture> texture) { m_Textures.push_back(texture); }
    const std::vector<std::shared_ptr<Texture>>& GetTextures() const { return m_Textures; }
    
private:
    void SetupMesh();
    
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    
    unsigned int m_VAO, m_VBO, m_EBO;
    glm::mat4 m_Transform;
};

}
