
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

std::optional<Window> Window::make(int x, int y, const std::string& name)
{

	if (!glfwInit()) {
		return std::nullopt;
	}

	GLFWwindow* w_ = glfwCreateWindow(x, y, name.c_str(), NULL, NULL);

	if (nullptr == w_) {
		return std::nullopt;
	}

	return std::optional<Window>(w_ );
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

void Window::ColorWindow(const vec4& Color)
{
	glClearColor(Color[0], Color[1], Color[2], Color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::UpdateWindow()
{
	glfwSwapBuffers(GetWindow());
	glfwPollEvents();
}

