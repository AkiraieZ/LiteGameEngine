#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace LGE {

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    
    void Bind() const;
    void Unbind() const;
    
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform2f(const std::string& name, float v1, float v2);
    void SetUniform3f(const std::string& name, float v1, float v2, float v3);
    void SetUniform4f(const std::string& name, float v1, float v2, float v3, float v4);
    void SetUniformVec3(const std::string& name, const glm::vec3& vec);
    void SetUniformVec4(const std::string& name, const glm::vec4& vec);
    void SetUniformMat4(const std::string& name, const glm::mat4& mat);
    
    unsigned int GetRendererID() const { return m_RendererID; }
    
private:
    std::string ReadFile(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    void CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation(const std::string& name);
    
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
};

}
