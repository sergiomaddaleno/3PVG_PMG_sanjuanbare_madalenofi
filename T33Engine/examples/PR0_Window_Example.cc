
#include "Core/window.h"
#include "Core/Input.h"

#include <../deps/glm/glm.hpp>
#include <optional>
#include <string>

glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

int main(int, char**) {

	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	//window->initGLEW();

	Input input { window->getWindow() };

	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);

		window->updateWindow();
		window->colorWindow(color);




		if (window->closePressed(input)) done = true;
	}

	return 0;
}
