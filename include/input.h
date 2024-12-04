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
        LEFT,
        RIGHT,
        UP,
        DOWN,
        CTRL,
        ALT,
        ENTER,
        ESC,
        SPACEBAR,
        BACKSPACE,
        W,
        A,
        S,
        D,
        E,
        Q,
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

    // Almacena las posiciones X e Y del ratón
    double mouse_x, mouse_y;

    // Diccionario de cada ventana con su input
    static std::unordered_map<GLFWwindow*, Input*> input_map;
    static std::unordered_map<int, Keys> keyGLFW_map;

public:
    Input(GLFWwindow* window) : window_{ window } {
        keys.assign(Keys_End, false);
        mouse_buttons.assign(MouseButton_End, 0);
        InitKeyMap();

        input_map.emplace(window_, this);
        glfwSetKeyCallback(window_, global_key_callback);
        glfwSetMouseButtonCallback(window_, global_mouse_button_callback);
        glfwSetCursorPosCallback(window_, global_cursor_pos_callback);
    }

    ~Input() {
        glfwSetKeyCallback(window_, nullptr);
        glfwSetMouseButtonCallback(window_, nullptr);
        glfwSetCursorPosCallback(window_, nullptr);
        input_map.erase(window_);
    }

    void InitKeyMap();

    static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void global_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

    void pollEvents();
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void cursor_pos_callback(double xpos, double ypos);

    bool isKeyDown(Keys key);
    bool isKeyPressed(Keys key);
    bool isKeyReleased(Keys key);

    bool isMouseButtonDown(MouseButton button);
    bool isMouseButtonPressed(MouseButton button);
    bool isMouseButtonReleased(MouseButton button);

    // Métodos para obtener la posición del ratón
    double getMouseX() const { return mouse_x; }
    double getMouseY() const { return mouse_y; }
};

#endif
