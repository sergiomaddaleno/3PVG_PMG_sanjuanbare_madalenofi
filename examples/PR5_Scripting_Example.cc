
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
	ls.run(lua_file);

	bool done = false;
	while (!done) {
		window->updateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
