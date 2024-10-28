#include "..\include\input.h"

std::unordered_map<GLFWwindow*, Input*> Input::input_map;
std::unordered_map<int, Input::Keys> Input::keyGLFW_map;

void Input::InitKeyMap(){

	keyGLFW_map[GLFW_KEY_UP] = Keys::UP;
	keyGLFW_map[GLFW_KEY_DOWN] = Keys::DOWN;
	keyGLFW_map[GLFW_KEY_LEFT] = Keys::LEFT;
	keyGLFW_map[GLFW_KEY_RIGHT] = Keys::RIGHT;
}

void Input::global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	input_map[window]->key_callback(key, scancode, action, mods);
}

void Input::pollEvents()
{
	glfwPollEvents();
}


void Input::key_callback(int key, int scancode, int action, int mods)
{
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

bool Input::isKeyDown(Keys key)
{
	return keys[static_cast<int>(key)];
}

bool Input::isKeyPressed(Keys key)
{
	static std::vector<bool> previousKeys(Keys_End, false);

	bool currentlyPressed = keys[static_cast<int>(key)];
	bool wasPressed = previousKeys[static_cast<int>(key)];

	// Actualizar el estado anterior
	previousKeys[static_cast<int>(key)] = currentlyPressed;

	// Retorna true si la tecla fue presionada este ciclo (de no presionada a presionada)
	return currentlyPressed && !wasPressed;
}

bool Input::isKeyReleased(Keys key)
{
	return !keys[static_cast<int>(key)];
}


