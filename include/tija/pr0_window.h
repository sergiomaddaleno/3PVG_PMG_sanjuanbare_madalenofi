
#ifndef PR0
#define PR0

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

class WindowSystem {

public:

	static std::optional<WindowSystem> make() {
		if (!glfwInit()) {
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

	static std::optional<Window> make(int x, int y, std::string& name, WindowSystem& /*Doesnt do anything but helps user*/) {
		auto loc_window = glfwCreateWindow(x, y, name.c_str(), nullptr, nullptr);
		if (nullptr == loc_window) return std::nullopt;

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

private:
	Window() {}

};


#endif // PR0
