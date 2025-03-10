/**
 * @file window.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __WINDOW__
#define __WINDOW__

#include <optional>
#include <string>
#include <Core/input.h>
#include <../deps/glm/glm.hpp>

 /**
  * @class WindowSystem
  * @brief Manages initialization and cleanup of the global windowing system.
  */
class WindowSystem {

public:

	static std::optional<WindowSystem> make();

	~WindowSystem() { glfwTerminate(); }

private: 
	WindowSystem() {}
};

/**
 * @class Window
 * @brief Encapsulates an OpenGL window using GLFW and handles window-related operations.
 */
class Window {

public:
	GLFWwindow* window_;

	Window(GLFWwindow* window) : width_(800), height_(600) { window_ = window; }

	static std::optional<Window> make(int x, int y, const std::string& name, bool full_screen = false);

	void setWindowActive();

	bool initGLEW();

	void updateWindow();

	//---
	void colorWindow(const glm::vec4& Color);

	void setWindowIcon(GLFWwindow* window, std::string path);

	void setResizeCallback();

	bool toggleFullScreen(Input &input, Input::Keys key = Input::Keys::F11, bool imgui_enter=false);

	bool closePressed(Input& input, Input::Keys key = Input::Keys::ESC);

	double getTime();

	GLFWwindow* getWindow() const { return window_; }

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

	void setWidth(int width) { width_ = width; }
	void setHeight(int height) { height_ = height; }

	//---

	Window(Window&& other) noexcept;
	~Window();

private:

	int width_ = 0;
	int height_ = 0;
	bool full_screen = false;

	//Window(GLFWwindow* window) : window_(window) {}
	//Window(const Window&) = delete;
	//Window& operator=(const Window&) = delete;
};


#endif // !__WINDOW__
