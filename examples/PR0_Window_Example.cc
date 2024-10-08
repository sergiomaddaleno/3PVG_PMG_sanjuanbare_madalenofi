
#include "tija/PR0_Window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>


void glfwErrorCallback(int error, const char* description) {
	printf("GLFW Error (%d): %s\n", error, description);
}

int main(int, char**) {

	glfwSetErrorCallback(glfwErrorCallback);

	//---
	glfwInit();
	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//---

	bool done = false;
	while (!done) {

		window->ProcessWindowEvents(window->GetWindow());

		bool close_pressed = window->closePressed();
		if (close_pressed) done = true;

	}

	return 0;
}
