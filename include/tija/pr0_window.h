
#ifndef PR0
#define PR0

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

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

	void SetWindowActive() { glfwMakeContextCurrent(window_); }

	void ProcessWindowEvents(GLFWwindow* window){
		glfwSwapBuffers(window); 
		glfwPollEvents();
	}

	GLFWwindow* GetWindow() const { return window_; }

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
