
#ifndef PR1
#define PR1

#include <GLFW/glfw3.h>
#include <optional>
#include <string>


class Triangle {

public:
	 struct InfoTriangles {
		float x, y;
		float r, g, b;
	} ;

	 InfoTriangles My_triangle[3];



	// Constructor that accepts an array of structs
	Triangle(InfoTriangles t[3]) {
		// Copy each element into InfoTriangles
		for (int i = 0; i < 3; i++) {
			My_triangle[i] = t[i];
		}
	}
	



	/*
	Window(GLFWwindow* window) { window_ = window; }

	static std::optional<Window> make(int x, int y, const std::string& name) {

		GLFWwindow* w_ = glfwCreateWindow(x, y, name.c_str(), NULL, NULL);

		if (nullptr == w_){
			return std::nullopt;
		}

		return std::optional<Window> { w_ };
	}

	GLFWwindow* GetWindow(){
		return window_;
	}

	void SetWindowActive() {
		glfwMakeContextCurrent(window_);
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
	//Window(GLFWwindow* window) : window_(window) {}
	//Window(const Window&) = delete;
	//Window& operator=(const Window&) = delete;
	*/
};


#endif // PR0
