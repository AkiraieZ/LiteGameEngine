#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace LGE {

class Input {
public:
    static bool IsKeyPressed(int keycode);
    static bool IsMouseButtonPressed(int button);
    static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
    
    static void Init(GLFWwindow* window);
    static void Shutdown();
    
    static void OnKey(int key, int action);
    static void OnMouseButton(int button, int action);
    static void OnMouseMove(double x, double y);
    static void OnMouseScroll(double xoffset, double yoffset);
    
private:
    static GLFWwindow* s_Window;
    static bool s_Keys[GLFW_KEY_LAST + 1];
    static bool s_MouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];
    static float s_MouseX, s_MouseY;
    static float s_ScrollX, s_ScrollY;
};

}
