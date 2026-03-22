#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace LGE {

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    
    Vertex() : Position(0.0f), Normal(0.0f), TexCoords(0.0f), Tangent(0.0f), Bitangent(0.0f) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm = glm::vec3(0.0f), 
           const glm::vec2& tex = glm::vec2(0.0f))
        : Position(pos), Normal(norm), TexCoords(tex), Tangent(0.0f), Bitangent(0.0f) {}
};

}
