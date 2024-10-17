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

void Input::key_callback(int key, int scancode, int action, int mods)
{
	keys[keyGLFW_map[key]] = (action != GLFW_RELEASE);
}

bool Input::isKeyPressed(Keys key)
{
	return keys[static_cast<int>(key)];
}

bool Input::isKeyReleased(Keys key)
{
	return !keys[static_cast<int>(key)];
}


