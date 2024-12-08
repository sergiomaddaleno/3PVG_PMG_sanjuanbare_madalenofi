
#include "window.h"
#include "lua.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../deps/glm/glm.hpp>
#include <optional>
#include <string>

#include "scripting.h"
#include "utils.h"


int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	//---

	LuaScript ls;
	auto lua_file = ReadFile("../scripts/test.lua");
	ls.add_global(multiply, "multiply");
	ls.run(lua_file);

	double lastTime = glfwGetTime();
	bool done = false;
	while (!done) {
		double currentTime = glfwGetTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();



		if (window->closePressed()) done = true;
	}

	return 0;
}
