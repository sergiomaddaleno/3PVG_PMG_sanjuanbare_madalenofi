
#include "window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

#include <../deps/linmath.h>

vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();
	//---

	bool done = false;
	while (!done) {
		window->ColorWindow(color);

		window->UpdateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
