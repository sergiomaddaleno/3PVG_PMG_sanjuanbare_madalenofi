/**
 * @file input.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __INPUT__
#define __INPUT__

#include <optional>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Input
  * @brief Handles user input for keyboard, mouse, and gamepad.
  */
class Input {

public:
    enum Keys {
        UP, DOWN, LEFT, RIGHT,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        CTRL, SHIFT, ALT, LSHIFT, RSHIFT, LCTRL, RCTRL, LALT, RALT,
        ENTER, ESC, SPACEBAR, BACKSPACE, TAB, CAPSLOCK, SUPR, INSERT, HOME, END, PAGEUP, PAGEDOWN, PRINTSCREEN, SCROLLLOCK, PAUSE,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
        NUMLOCK, NUMPLUS, NUMMINUS, NUMMULTIPLY, NUMDIVIDE, NUMDECIMAL, NUMENTER,
        Keys_End, None
    };

    enum MouseButton {
        MOUSE_LEFT,
        MOUSE_RIGHT,
        MOUSE_MIDDLE,
        MouseButton_End
    };

    enum GamePad {
        BUTTON_X, BUTTON_SQUARE, BUTTON_TRIANGLE, BUTTON_CIRCLE,
        DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT,
        L1, R1, L2, R2,
        L3, R3,
        START, SELECT, GP_HOME,
        GamePad_End, GamePad_None
    };

    Input(GLFWwindow* window);
    ~Input();

    bool isKeyDown(Keys key);
    bool isKeyPressed(Keys key);
    bool isKeyReleased(Keys key);

    bool isMouseButtonDown(MouseButton button);
    bool isMouseButtonPressed(MouseButton button);
    bool isMouseButtonReleased(MouseButton button);

    double getMouseX() const { return mouse_x; }
    double getMouseY() const { return mouse_y; }

    double getScrollX() const { return scroll_x; }
    double getScrollY() const { return scroll_y; }
    void resetScroll();

    static std::unordered_map<std::string, Input::Keys> stringMap();

private:

    void InitKeyMap();
    void InitMouseMap();
    void InitGamePadMap();

    static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void global_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void pollEvents();
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void cursor_pos_callback(double xpos, double ypos);
    void scroll_callback(double xpos, double ypos);

    GLFWwindow* window_;
    std::map<int, Input> action_map;
    std::vector<int> keys{ Keys_End };
    std::vector<int> mouse_buttons{ MouseButton_End };
    std::vector<int> gamepad_buttons{ GamePad_End };

    static std::unordered_map<GLFWwindow*, Input*> input_map;

    static std::unordered_map<int, Keys> keyGLFW_map;
    static std::unordered_map<int, MouseButton> MouseGLFW_map;
    static std::unordered_map<int, GamePad> GamePadGLFW_map;

    double mouse_x, mouse_y;
    double scroll_x, scroll_y;
};

#endif // !__INPUT__
