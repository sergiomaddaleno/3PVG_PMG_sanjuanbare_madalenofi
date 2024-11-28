
#ifndef PR0
#define PR0

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
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

	Window(GLFWwindow* window) { window_ = window; }

	static std::optional<Window> make(int x, int y, const std::string& name);

	bool closePressed();

	void setWindowActive() { glfwMakeContextCurrent(window_); }

	void colorWindow(const glm::vec4& Color);

	void updateWindow();

	GLFWwindow* getWindow() const { return window_; }

	Window(Window&& other) {
		window_ = other.window_;
		other.window_ = nullptr;
	}

	~Window() {
		if (nullptr != window_) {
			glfwDestroyWindow(window_);
		}
	}

private:
	//Window(GLFWwindow* window) : window_(window) {}
	//Window(const Window&) = delete;
	//Window& operator=(const Window&) = delete;
};


#endif // PR0
