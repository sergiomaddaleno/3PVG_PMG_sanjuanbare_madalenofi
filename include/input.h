#ifndef INP
#define INP

#include <optional>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input {

public:
    enum Keys {
        UP, DOWN, LEFT, RIGHT,
        W,A,S,D,E,Q,

        CTRL,
        SHIFT,
        F11,

        ALT,
        ENTER,
        ESC,
        SPACEBAR,
        BACKSPACE,

        Keys_End
    };

    enum MouseButton {
        MOUSE_LEFT = 0,
        MOUSE_RIGHT = 1,
        MOUSE_MIDDLE = 2,
        MouseButton_End
    };

private:
    GLFWwindow* window_;
    std::map<int, Input> action_map;
    std::vector<int> keys{ Keys_End };
    std::vector<int> mouse_buttons{ MouseButton_End };

    // Diccionario de cada ventana con su input
    static std::unordered_map<GLFWwindow*, Input*> input_map;
    static std::unordered_map<int, Keys> keyGLFW_map;


    double mouse_x, mouse_y;
    double scroll_x, scroll_y;

public:
    Input(GLFWwindow* window);

    ~Input();

    void InitKeyMap();

    static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void global_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void pollEvents();
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void cursor_pos_callback(double xpos, double ypos);
    void scroll_callback(double xpos, double ypos);

    // ---

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
};

#endif
