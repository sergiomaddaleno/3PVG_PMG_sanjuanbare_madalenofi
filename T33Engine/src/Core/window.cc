
#include <Core/window.h>
#include "Core/texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>

Window::Window(Window&& other) noexcept
 {
	window_ = other.window_;
	other.window_ = nullptr;
}

Window::~Window()
{
	if (nullptr != window_) {
		glfwDestroyWindow(window_);
	}
}

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

		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		w_ = glfwCreateWindow(mode->width, mode->height, name.c_str(), monitor, NULL);
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

bool Window::toggleFullScreen(Input& input, Input::Keys key, bool imgui_enter)
{
	if (input.isKeyPressed(key) || imgui_enter)
	{
		if (!full_screen)
		{
			int xpos, ypos;
			glfwGetWindowPos(window_, &xpos, &ypos);
			int width, height;
			glfwGetWindowSize(window_, &width, &height);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			// fullscreen
			glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			full_screen = true;
		}
		else
		{
			// windowed mode
			glfwSetWindowMonitor(window_, NULL, 5, 40, 800, 600, 0);
			full_screen = false;
		}
		return true;
	}

	return false;
}

bool Window::closePressed(Input& input, Input::Keys key)
{
	if (input.isKeyPressed(key) || glfwWindowShouldClose(window_))
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

void Window::setWindowIcon(GLFWwindow* window, std::string path)
{
	Texture tex;
	if (!tex.loadTexture(path)) {
		printf("fail");
	}

	GLFWimage icon[1];
	icon[0].width = tex.getwidth();
	icon[0].height = tex.getHeight();
	icon[0].pixels = tex.getData();

	glfwSetWindowIcon(window, 1, icon);
}

void Window::updateWindow()
{

	glfwSwapBuffers(getWindow());
	glfwPollEvents();
}

void Window::setWindowActive()
{
	glfwMakeContextCurrent(window_);
}

bool Window::initGLEW()
{
	if (glewInit() != GLEW_OK) {
		return false;
	}

	return true;
}

double Window::getTime()
{
	return glfwGetTime();
}

