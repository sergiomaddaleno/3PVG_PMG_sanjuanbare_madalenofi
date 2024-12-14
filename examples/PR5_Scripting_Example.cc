
#include "window.h"
#include "lua.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../deps/glm/glm.hpp>
#include <optional>
#include <string>

#include "ecs_manager.h"
#include "Components/scripting_component.h"
#include "utils.h"

ECSManager ecs;

auto lua_file = ReadFile("../scripts/test.lua");

int main(int, char**) {

	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	//--

	auto e = ecs.create_entity();
	ecs.add_component_type<ScriptingComponent>();

	ecs.add_component<ScriptingComponent>(e);
	auto sc = ecs.get_component<ScriptingComponent>(e);

	//Ejecutar funcion desde lua
	sc->lua_script->add_global(multiply, "multiply");
	sc->lua_script->run(lua_file);

	//Coger variables de lua
	int w_width = sc->lua_script->get_integer("window_width");
	int w_height = sc->lua_script->get_integer("window_height");



	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();


		if (window->closePressed()) done = true;
	}

	return 0;
}

/* EJEMPLO DE LUASCRIPT SIN USAR ECS

int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	//---

	LuaScript ls;
	auto lua_file = ReadFile("../scripts/test.lua");
	ls.add_global(multiply, "multiply");
	ls.run(lua_file);

	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();



		if (window->closePressed()) done = true;
	}

	return 0;
}*/
