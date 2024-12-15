
#ifndef PR0
#define PR0

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include <input.h>

#include <../deps/glm/glm.hpp>

class WindowSystem {

public:

	static std::optional<WindowSystem> make();

	~WindowSystem() { glfwTerminate(); }

private: 
	WindowSystem() {}
};

class Window {

public:
	GLFWwindow* window_;

	Window(GLFWwindow* window) : width_(800), height_(600) { window_ = window; }

	static std::optional<Window> make(int x, int y, const std::string& name, bool full_screen = false);

	void updateWindow();

	void setResizeCallback();

	void toggleFullScreen(Input &input, Input::Keys key = Input::Keys::F11);

	void colorWindow(const glm::vec4& Color);

	bool closePressed();

	//---
	void setWindowActive();

	double getTime();

	GLFWwindow* getWindow() const { return window_; }

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

	//---

	Window(Window&& other) noexcept {
		window_ = other.window_;
		other.window_ = nullptr;
	}

	~Window() {
		if (nullptr != window_) {
			glfwDestroyWindow(window_);
		}
	}

private:

	int width_ = 0;
	int height_ = 0;
	bool full_screen = false;

	int prev_x = 0, prev_y = 0;
	int prev_width = 0, prev_height = 0;

	//Window(GLFWwindow* window) : window_(window) {}
	//Window(const Window&) = delete;
	//Window& operator=(const Window&) = delete;
};


#endif // PR0
