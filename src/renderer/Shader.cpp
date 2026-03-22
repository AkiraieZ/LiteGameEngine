#include "Shader.h"
#include "../core/Logger.h"
#include <fstream>
#include <sstream>
#include <glad/gl.h>

namespace LGE {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);
    CreateShader(vertexSource, fragmentSource);
}

Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

std::string Shader::ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file: %s", filepath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    if (content.empty()) {
        LGE_LOG_ERROR("File is empty: %s", filepath.c_str());
    }
    return content;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        LGE_LOG_ERROR("Failed to compile %s shader!", 
                       (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
        LGE_LOG_ERROR("%s", message);
        delete[] message;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

void Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    m_RendererID = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(m_RendererID, vs);
    glAttachShader(m_RendererID, fs);
    glLinkProgram(m_RendererID);
    glValidateProgram(m_RendererID);
    
    int result;
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetProgramInfoLog(m_RendererID, length, &length, message);
        LGE_LOG_ERROR("Failed to link shader program!");
        LGE_LOG_ERROR("%s", message);
        delete[] message;
        glDeleteProgram(m_RendererID);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return;
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::Bind() const {
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

int Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }
    
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        LGE_LOG_WARNING("Uniform '%s' doesn't exist!", name.c_str());
    }
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::SetUniform1i(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform2f(const std::string& name, float v1, float v2) {
    glUniform2f(GetUniformLocation(name), v1, v2);
}

void Shader::SetUniform3f(const std::string& name, float v1, float v2, float v3) {
    glUniform3f(GetUniformLocation(name), v1, v2, v3);
}

void Shader::SetUniform4f(const std::string& name, float v1, float v2, float v3, float v4) {
    glUniform4f(GetUniformLocation(name), v1, v2, v3, v4);
}

void Shader::SetUniformVec3(const std::string& name, const glm::vec3& vec) {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(vec));
}

void Shader::SetUniformVec4(const std::string& name, const glm::vec4& vec) {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(vec));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

}
