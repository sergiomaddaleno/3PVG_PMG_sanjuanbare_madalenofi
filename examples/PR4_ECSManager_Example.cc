
#include "window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include "ecs_manager.h"

struct RenderTest {
	void myRenderprint();
};
struct PositionComponent {};

RenderTest rc;
PositionComponent pc;

void RenderTest::myRenderprint() {
	printf("print renderizado\n");
}

void renderSystem(std::vector<std::optional<RenderTest>>& rc) {
	auto ri = rc.begin();
	auto riend = rc.end();
	for (; ri != riend ; ri++) {
		// TODO OPENGL
		ri->value().myRenderprint();
	}
}


int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();

	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	//---

	ECSManager ecs;

	auto entidad_0 = ecs.create_entity();
	ecs.add_component_type<RenderTest>();
	ecs.get_component<RenderTest>(entidad_0);

	bool done = false;
	while (!done) {

		window->UpdateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
