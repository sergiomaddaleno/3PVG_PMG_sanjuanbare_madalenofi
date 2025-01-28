#include "Core/input.h"

std::unordered_map<GLFWwindow*, Input*> Input::input_map;
std::unordered_map<int, Input::Keys> Input::keyGLFW_map;

Input::Input(GLFWwindow* window) :
    mouse_x(0.0f),
    mouse_y(0.0f),
    scroll_x(0.0f),
    scroll_y(0.0f),
    window_{ window }
{
    keys.assign(Keys_End, false);
    mouse_buttons.assign(MouseButton_End, 0);
    InitKeyMap();

    input_map.emplace(window_, this);
    glfwSetKeyCallback(window_, global_key_callback);
    glfwSetMouseButtonCallback(window_, global_mouse_button_callback);
    glfwSetCursorPosCallback(window_, global_cursor_pos_callback);
    glfwSetScrollCallback(window_, Input::global_scroll_callback);
}

Input::~Input()
{
    glfwSetKeyCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);
    input_map.erase(window_);
}

void Input::InitKeyMap() {
    keyGLFW_map[GLFW_KEY_UP] = Keys::UP;
    keyGLFW_map[GLFW_KEY_DOWN] = Keys::DOWN;
    keyGLFW_map[GLFW_KEY_LEFT] = Keys::LEFT;
    keyGLFW_map[GLFW_KEY_RIGHT] = Keys::RIGHT;
    keyGLFW_map[GLFW_KEY_W] = Keys::W;
    keyGLFW_map[GLFW_KEY_A] = Keys::A;
    keyGLFW_map[GLFW_KEY_S] = Keys::S;
    keyGLFW_map[GLFW_KEY_D] = Keys::D;
    keyGLFW_map[GLFW_KEY_Q] = Keys::Q;
    keyGLFW_map[GLFW_KEY_E] = Keys::E;

    keyGLFW_map[GLFW_KEY_LEFT_CONTROL] = Keys::CTRL;
    keyGLFW_map[GLFW_KEY_RIGHT_CONTROL] = Keys::CTRL;
    keyGLFW_map[GLFW_KEY_LEFT_SHIFT] = Keys::SHIFT;
    keyGLFW_map[GLFW_KEY_RIGHT_SHIFT] = Keys::SHIFT;
    keyGLFW_map[GLFW_KEY_DELETE] = Keys::SUPR;

    keyGLFW_map[GLFW_KEY_F11] = Keys::F11;
}

void Input::global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    input_map[window]->key_callback(key, scancode, action, mods);
}

void Input::global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    input_map[window]->mouse_button_callback(button, action, mods);
}

void Input::global_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    input_map[window]->cursor_pos_callback(xpos, ypos);
}

void Input::global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    input_map[window]->scroll_callback(xoffset, yoffset);
}

void Input::pollEvents() {
    glfwPollEvents();
}

void Input::key_callback(int key, int scancode, int action, int mods) {
    if (keyGLFW_map.find(key) != keyGLFW_map.end()) {
        Keys mappedKey = keyGLFW_map[key];
        if (action == GLFW_PRESS) {
            keys[static_cast<int>(mappedKey)] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[static_cast<int>(mappedKey)] = false;
        }
    }
}

void Input::mouse_button_callback(int button, int action, int mods) {
    if (button >= 0 && button < MouseButton_End) {
        mouse_buttons[button] = (action == GLFW_PRESS) ? 1 : 0;
    }
}

void Input::cursor_pos_callback(double xpos, double ypos) {
    mouse_x = xpos;  
    mouse_y = ypos;  
}

void Input::scroll_callback(double xoffset, double yoffset) {
    scroll_x = xoffset;
    scroll_y = yoffset;
}

void Input::resetScroll() {
    scroll_x = 0.0f;
    scroll_y = 0.0f;
}

bool Input::isKeyDown(Keys key) {
    return keys[static_cast<int>(key)];
}

bool Input::isKeyPressed(Keys key) {
    static std::vector<bool> previousKeys(Keys_End, false);

    bool currentlyPressed = keys[static_cast<int>(key)];
    bool wasPressed = previousKeys[static_cast<int>(key)];

    
    previousKeys[static_cast<int>(key)] = currentlyPressed;

    
    return currentlyPressed && !wasPressed;
}

bool Input::isKeyReleased(Keys key) {
    return !keys[static_cast<int>(key)];
}

bool Input::isMouseButtonDown(MouseButton button) {
    return mouse_buttons[button] == 1;
}

bool Input::isMouseButtonPressed(MouseButton button) {
    static std::vector<int> previousMouseButtons(MouseButton_End, 0);

    bool currentlyPressed = mouse_buttons[button] == 1;
    bool wasPressed = previousMouseButtons[button] == 1;

    
    previousMouseButtons[button] = currentlyPressed;

    
    return currentlyPressed && !wasPressed;
}

bool Input::isMouseButtonReleased(MouseButton button) {
    return mouse_buttons[button] == 0;
}
