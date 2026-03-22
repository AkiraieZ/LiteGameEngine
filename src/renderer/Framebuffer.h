#pragma once

#include <vector>
#include <memory>

namespace LGE {

class Texture;

class Framebuffer {
public:
    struct FramebufferSpecification {
        uint32_t Width, Height;
        uint32_t Samples = 1;
        bool SwapChainTarget = false;
    };
    
    Framebuffer(const FramebufferSpecification& spec);
    ~Framebuffer();
    
    void Bind();
    void Unbind();
    
    void Resize(uint32_t width, uint32_t height);
    void Invalidate();
    
    uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { return m_ColorAttachment; }
    uint32_t GetDepthAttachmentRendererID() const { return m_DepthAttachment; }
    
    const FramebufferSpecification& GetSpecification() const { return m_Specification; }
    
    void ClearAttachment(uint32_t attachmentIndex, int value);
    int ReadPixel(uint32_t attachmentIndex, int x, int y);
    
private:
    uint32_t m_RendererID;
    FramebufferSpecification m_Specification;
    uint32_t m_ColorAttachment = 0;
    uint32_t m_DepthAttachment = 0;
};

}
