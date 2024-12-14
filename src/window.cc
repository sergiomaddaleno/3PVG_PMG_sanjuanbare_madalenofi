
#include <../include/window.h>
#include <optional>

std::optional<WindowSystem> WindowSystem::make()
{
	WindowSystem ws_;

	if (!glfwInit()) {
		return std::nullopt;
	}

	return std::optional<WindowSystem>( ws_);
}

std::optional<Window> Window::make(int x, int y, const std::string& name, bool full_screen)
{

	if (!glfwInit()) {
		return std::nullopt;
	}

	GLFWwindow* w_;

	if (!full_screen) {
		w_ = glfwCreateWindow(x, y, name.c_str(), NULL, NULL);
	}
	else {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		w_ = glfwCreateWindow(mode->width, mode->height, name.c_str(), NULL, NULL);
	}

	if (nullptr == w_) {
		return std::nullopt;
	}

	return std::optional<Window>(w_ );
}

void Window::setResizeCallback() {
	glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);

		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win) {
			win->width_ = width;
			win->height_ = height;
		}
	});

	glfwGetFramebufferSize(window_, &width_, &height_);
	glfwSetWindowUserPointer(window_, this);
}

bool Window::closePressed()
{
	int state = glfwGetKey(window_, GLFW_KEY_ESCAPE);

	if (state == GLFW_PRESS || glfwWindowShouldClose(window_))
	{
		return true;
	}

	return false;
}

void Window::colorWindow(const glm::vec4& Color)
{
	glClearColor(Color[0], Color[1], Color[2], Color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::updateWindow()
{
	glfwSwapBuffers(getWindow());
	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Window::setWindowActive()
{
	glfwMakeContextCurrent(window_);
}

double Window::getTime()
{
	return glfwGetTime();
}

