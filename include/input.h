
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
		Keys_End
	};

private:
	GLFWwindow* window_;
	std::map<int, Input> action_map;
	std::vector<int> keys{ Keys_End };

	//Diccionario de cada ventana con su input
	static std::unordered_map<GLFWwindow*, Input*> input_map;
	//Asignar a mis keys las de glfw 
	static std::unordered_map<int, Keys> keyGLFW_map;

public:

	Input(GLFWwindow* window) : window_{ window } {

		keys.assign(Keys_End, false);
		InitKeyMap();

		input_map.emplace(window_, this);
		glfwSetKeyCallback(window_, global_key_callback);
	}

	~Input() {
		glfwSetKeyCallback(window_, nullptr);
		input_map.erase(window_);
	}

	//---

	void InitKeyMap();

	static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void pollEvents();

	void key_callback(int key, int scancode, int action, int mods);
	
	//bool isKeyUp(Keys key);
	bool isKeyDown(Keys key);
	bool isKeyPressed(Keys key);
	bool isKeyReleased(Keys key);
};


#endif

