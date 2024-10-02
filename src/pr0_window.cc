
#include "tija/pr0_window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>

int main(int, char**) {

	//---
	auto maybe_ws = WindowSystem::make();
	if (!maybe_ws) { return -1; }
	
	auto ws = maybe_ws.get();

	//---
	auto maybe_w = Window::make{640,480,"Window",ws };
	if (!maybe_w) {return -1; }
	auto w = maybe_w.get();

	bool done = false;
	while (!done) {
		bool closePressed = w.closePressed();
	
		if (closePressed) done = true;
	}

	return 0;
}
