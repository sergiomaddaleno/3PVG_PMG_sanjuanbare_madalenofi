
#ifndef PR0
#define PR0

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

class WindowSystem {

public:

	static std::optional<WindowSystem> make() {

		if (!glfwInit()) {
			printf("Failed\n");
			return std::nullopt;
		}
		return WindowSystem{};
	}

	~WindowSystem() {
		glfwTerminate();
	}

private:
	WindowSystem() {}
};

class Window {

public:
	GLFWwindow* window_;

	static std::optional<Window> make(int x, int y, const std::string& name, WindowSystem& /*Doesnt do anything but helps user*/) {

		auto loc_window = glfwCreateWindow(x, y, name.c_str(), NULL, NULL);
		if (nullptr == loc_window){
			return std::nullopt;
		}

		return Window{ loc_window };
	}

	Window(Window&& other) {
		window_ = other.window_;
		other.window_ = nullptr;
	}

	~Window() {
		if (nullptr != window_) {
			glfwDestroyWindow(window_);
		}
	}

	bool closePressed() {
		//return glfwSetWindowShouldClose(window_, GLFW_FALSE);

		/*
		int state = glfwGetKey(window_, GLFW_KEY_E);

		if (state == GLFW_PRESS)
		{
			return true;
		}

		*/
		return false;
	}

private:
	Window(GLFWwindow* window) : window_(window) {}
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
};


#endif // PR0
