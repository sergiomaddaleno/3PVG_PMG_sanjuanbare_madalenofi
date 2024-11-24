
#include "window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>


int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	//---

	bool done = false;
	while (!done) {

		window->UpdateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
