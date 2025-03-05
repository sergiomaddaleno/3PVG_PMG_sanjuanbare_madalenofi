
#include "Core/input.h"

std::unordered_map<GLFWwindow*, Input*> Input::input_map;
std::unordered_map<int, Input::Keys> Input::keyGLFW_map;
std::unordered_map<int, Input::MouseButton> Input::MouseGLFW_map;
std::unordered_map<int, Input::GamePad> Input::GamePadGLFW_map;

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
    InitMouseMap();
    InitGamePadMap();

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

    for (int i = GLFW_KEY_A; i <= GLFW_KEY_Z; i++) {
        keyGLFW_map[i] = static_cast<Keys>(Keys::A + (i - GLFW_KEY_A));
    }

    keyGLFW_map[GLFW_KEY_LEFT_CONTROL] = Keys::LCTRL;
    keyGLFW_map[GLFW_KEY_RIGHT_CONTROL] = Keys::RCTRL;
    keyGLFW_map[GLFW_KEY_LEFT_SHIFT] = Keys::LSHIFT;
    keyGLFW_map[GLFW_KEY_RIGHT_SHIFT] = Keys::RSHIFT;
    keyGLFW_map[GLFW_KEY_LEFT_ALT] = Keys::LALT;
    keyGLFW_map[GLFW_KEY_RIGHT_ALT] = Keys::RALT;

    keyGLFW_map[GLFW_KEY_ENTER] = Keys::ENTER;
    keyGLFW_map[GLFW_KEY_ESCAPE] = Keys::ESC;
    keyGLFW_map[GLFW_KEY_SPACE] = Keys::SPACEBAR;
    keyGLFW_map[GLFW_KEY_BACKSPACE] = Keys::BACKSPACE;
    keyGLFW_map[GLFW_KEY_TAB] = Keys::TAB;
    keyGLFW_map[GLFW_KEY_CAPS_LOCK] = Keys::CAPSLOCK;
    keyGLFW_map[GLFW_KEY_DELETE] = Keys::SUPR;
    keyGLFW_map[GLFW_KEY_INSERT] = Keys::INSERT;
    keyGLFW_map[GLFW_KEY_HOME] = Keys::HOME;
    keyGLFW_map[GLFW_KEY_END] = Keys::END;
    keyGLFW_map[GLFW_KEY_PAGE_UP] = Keys::PAGEUP;
    keyGLFW_map[GLFW_KEY_PAGE_DOWN] = Keys::PAGEDOWN;
    keyGLFW_map[GLFW_KEY_PRINT_SCREEN] = Keys::PRINTSCREEN;
    keyGLFW_map[GLFW_KEY_SCROLL_LOCK] = Keys::SCROLLLOCK;
    keyGLFW_map[GLFW_KEY_PAUSE] = Keys::PAUSE;

    for (int i = GLFW_KEY_F1; i <= GLFW_KEY_F12; i++) {
        keyGLFW_map[i] = static_cast<Keys>(Keys::F1 + (i - GLFW_KEY_F1));
    }

    for (int i = GLFW_KEY_0; i <= GLFW_KEY_9; i++) {
        keyGLFW_map[i] = static_cast<Keys>(Keys::NUM0 + (i - GLFW_KEY_0));
    }

    keyGLFW_map[GLFW_KEY_NUM_LOCK] = Keys::NUMLOCK;
    keyGLFW_map[GLFW_KEY_KP_ADD] = Keys::NUMPLUS;
    keyGLFW_map[GLFW_KEY_KP_SUBTRACT] = Keys::NUMMINUS;
    keyGLFW_map[GLFW_KEY_KP_MULTIPLY] = Keys::NUMMULTIPLY;
    keyGLFW_map[GLFW_KEY_KP_DIVIDE] = Keys::NUMDIVIDE;
    keyGLFW_map[GLFW_KEY_KP_DECIMAL] = Keys::NUMDECIMAL;
    keyGLFW_map[GLFW_KEY_KP_ENTER] = Keys::NUMENTER;

    for (int i = GLFW_KEY_KP_0; i <= GLFW_KEY_KP_9; i++) {
        keyGLFW_map[i] = static_cast<Keys>(Keys::NUM0 + (i - GLFW_KEY_KP_0));
    }
}

void Input::InitMouseMap()
{
    MouseGLFW_map[GLFW_MOUSE_BUTTON_LEFT] = MouseButton::MOUSE_LEFT;
    MouseGLFW_map[GLFW_MOUSE_BUTTON_RIGHT] = MouseButton::MOUSE_RIGHT;
    MouseGLFW_map[GLFW_MOUSE_BUTTON_MIDDLE] = MouseButton::MOUSE_MIDDLE;
}

void Input::InitGamePadMap()
{
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_A] = GamePad::BUTTON_X; 
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_X] = GamePad::BUTTON_SQUARE;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_Y] = GamePad::BUTTON_TRIANGLE;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_B] = GamePad::BUTTON_CIRCLE;

    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_DPAD_UP] = GamePad::DPAD_UP;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] = GamePad::DPAD_DOWN;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = GamePad::DPAD_LEFT;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = GamePad::DPAD_RIGHT;

    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] = GamePad::L1;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = GamePad::R1;

    GamePadGLFW_map[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = GamePad::L2;
    GamePadGLFW_map[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = GamePad::R2;

    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = GamePad::L3;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] = GamePad::R3;

    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_START] = GamePad::START;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_BACK] = GamePad::SELECT;
    GamePadGLFW_map[GLFW_GAMEPAD_BUTTON_GUIDE] = GamePad::GP_HOME;
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
