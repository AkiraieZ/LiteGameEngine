#include "Window.h"
#include "../core/Logger.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace LGE {

static bool s_GLFWInitialized = false;

Window::Window(const WindowProps& props) {
    Init(props);
}

Window::~Window() {
    Shutdown();
}

void Window::Init(const WindowProps& props) {
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;
    m_Data.VSync = props.VSync;
    
    LGE_LOG_INFO("Creating window %s (%d, %d)", props.Title.c_str(), props.Width, props.Height);
    
    if (!s_GLFWInitialized) {
        int success = glfwInit();
        if (!success) {
            LGE_LOG_ERROR("Failed to initialize GLFW!");
            return;
        }
        s_GLFWInitialized = true;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        LGE_LOG_ERROR("Failed to create GLFW window!");
        return;
    }
    
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(props.VSync);
    
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        LGE_LOG_ERROR("Failed to initialize GLAD!");
        return;
    }
    
    LGE_LOG_INFO("OpenGL Info:");
    LGE_LOG_INFO("  Vendor: %s", glGetString(GL_VENDOR));
    LGE_LOG_INFO("  Renderer: %s", glGetString(GL_RENDERER));
    LGE_LOG_INFO("  Version: %s", glGetString(GL_VERSION));
    
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (data.EventCallback) {
            data.EventCallback(0, 0, 0, 0);
        }
    });
    
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;
        glViewport(0, 0, width, height);
    });
    
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (data.EventCallback) {
            data.EventCallback(1, key, scancode, action);
        }
    });
    
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (data.EventCallback) {
            data.EventCallback(2, button, action, mods);
        }
    });
    
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (data.EventCallback) {
            data.EventCallback(3, (int)xpos, (int)ypos, 0);
        }
    });
}

void Window::Shutdown() {
    glfwDestroyWindow(m_Window);
    
    if (s_GLFWInitialized) {
        glfwTerminate();
        s_GLFWInitialized = false;
    }
}

void Window::OnUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void Window::SetVSync(bool enabled) {
    if (enabled) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
    m_Data.VSync = enabled;
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::Close() {
    glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
}

}
