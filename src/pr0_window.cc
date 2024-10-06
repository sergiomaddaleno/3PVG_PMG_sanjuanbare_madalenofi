
#include "tija/pr0_window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

void glfwErrorCallback(int error, const char* description) {
	printf("GLFW Error (%d): %s\n", error, description);
}

int main(int, char**) {

	glfwSetErrorCallback(glfwErrorCallback);

	//---
	auto maybe_ws = WindowSystem::make();
	if (!maybe_ws) { printf("ws");  return -1;  }
	
	auto ws = maybe_ws.value();

	//---
	auto maybe_w = Window::make(640,480,"Window",ws );

	if (!maybe_w) { printf("ENTRO");  return -1; }
	auto w = std::move(maybe_w.value());

	bool done = false;
	while (!done) {
		bool closePressed = w.closePressed();
	
		if (closePressed) done = true;
	}

	return 0;
}
