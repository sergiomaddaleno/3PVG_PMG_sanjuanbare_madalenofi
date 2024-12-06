
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

	Window(GLFWwindow* window) : width_(800), height_(600) { window_ = window; }

	static std::optional<Window> make(int x, int y, const std::string& name);

	void setResizeCallback();

	bool closePressed();

	void setWindowActive() { glfwMakeContextCurrent(window_); }

	void colorWindow(const glm::vec4& Color);

	void updateWindow();

	GLFWwindow* getWindow() const { return window_; }

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

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

	int width_;
	int height_;
	//Window(GLFWwindow* window) : window_(window) {}
	//Window(const Window&) = delete;
	//Window& operator=(const Window&) = delete;
};


#endif // PR0
