
#include "tija/pr0_window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>
/*
void glfwErrorCallback(int error, const char* description) {
	printf("GLFW Error (%d): %s\n", error, description);
}

int main(int, char**) {

	glfwSetErrorCallback(glfwErrorCallback);

	//---
	glfwInit();
	auto window = Window::make(800, 600, "Window");
	//---

	window->SetWindowActive();

	bool done = false;
	while (!done) {
		
		glfwSwapBuffers(window->GetWindow());
		glfwPollEvents();
		
	}

	return 0;
}
*/