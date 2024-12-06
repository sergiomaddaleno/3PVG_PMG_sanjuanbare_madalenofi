
#include "window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../deps/glm/glm.hpp>
#include <optional>
#include <string>

glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	//---

	bool done = false;
	while (!done) {
		window->colorWindow(color);


		window->updateWindow();
		if (window->closePressed()) done = true;
	}

	return 0;
}
