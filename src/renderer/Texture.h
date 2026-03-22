#pragma once

#include <string>

namespace LGE {

class Texture {
public:
    Texture(const std::string& filepath);
    Texture(int width, int height, unsigned char* data = nullptr);
    ~Texture();
    
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;
    
    unsigned int GetRendererID() const { return m_RendererID; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    const std::string& GetPath() const { return m_FilePath; }
    
private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    int m_Width, m_Height, m_Channels;
};

}
