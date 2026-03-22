#include "Input.h"
#include <cstring>

namespace LGE {

GLFWwindow* Input::s_Window = nullptr;
bool Input::s_Keys[GLFW_KEY_LAST + 1] = { false };
bool Input::s_MouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = { false };
float Input::s_MouseX = 0.0f;
float Input::s_MouseY = 0.0f;
float Input::s_ScrollX = 0.0f;
float Input::s_ScrollY = 0.0f;

void Input::Init(GLFWwindow* window) {
    s_Window = window;
    memset(s_Keys, 0, sizeof(s_Keys));
    memset(s_MouseButtons, 0, sizeof(s_MouseButtons));
}

void Input::Shutdown() {
    s_Window = nullptr;
}

bool Input::IsKeyPressed(int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST) return false;
    return s_Keys[keycode];
}

bool Input::IsMouseButtonPressed(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return s_MouseButtons[button];
}

glm::vec2 Input::GetMousePosition() {
    return glm::vec2(s_MouseX, s_MouseY);
}

float Input::GetMouseX() {
    return s_MouseX;
}

float Input::GetMouseY() {
    return s_MouseY;
}

void Input::OnKey(int key, int action) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        s_Keys[key] = (action != GLFW_RELEASE);
    }
}

void Input::OnMouseButton(int button, int action) {
    if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
        s_MouseButtons[button] = (action != GLFW_RELEASE);
    }
}

void Input::OnMouseMove(double x, double y) {
    s_MouseX = (float)x;
    s_MouseY = (float)y;
}

void Input::OnMouseScroll(double xoffset, double yoffset) {
    s_ScrollX += (float)xoffset;
    s_ScrollY += (float)yoffset;
}

}
