#pragma once

#include <string>
#include <functional>

struct GLFWwindow;

namespace LGE {

using EventCallbackFn = std::function<void(int, int, int, int)>;

class Window {
public:
    struct WindowProps {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        bool VSync;
        bool Fullscreen;
        
        WindowProps(const std::string& title = "LightGameEngine",
                    uint32_t width = 1280,
                    uint32_t height = 720,
                    bool vsync = true,
                    bool fullscreen = false)
            : Title(title), Width(width), Height(height), VSync(vsync), Fullscreen(fullscreen) {}
    };
    
    explicit Window(const WindowProps& props = WindowProps());
    ~Window();
    
    void OnUpdate();
    
    uint32_t GetWidth() const { return m_Data.Width; }
    uint32_t GetHeight() const { return m_Data.Height; }
    
    void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled);
    bool IsVSync() const { return m_Data.VSync; }
    
    bool ShouldClose() const;
    void Close();
    
    GLFWwindow* GetNativeWindow() const { return m_Window; }
    
private:
    void Init(const WindowProps& props);
    void Shutdown();
    
    struct WindowData {
        std::string Title;
        uint32_t Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };
    
    GLFWwindow* m_Window;
    WindowData m_Data;
};

}
