
#include "mesh.h"
#include "render.h"

#include "window.h"
#include <optional>
#include <string>

#include "utils.h"
#include <../deps/glm/glm.hpp>

std::shared_ptr<Mesh> mesh_ = std::make_shared<Mesh>();

Render render_;
char* vs_normals = (char*)ReadFile("../assets/shaders/vs_normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/fs_normals.fs");

int main(int, char**) {
	
	//---
	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	window->initGLEW();
	//---

	if (!mesh_->loadOBJ("../assets/obj/SM_Teapot.obj")) {
		return -1;
	}

	RenderData rd;
	rd.render_comp.mesh_ = mesh_;
	rd.transf_comp.position = glm::vec3{ 0.0f, -2.0f, -5.0f };
	rd.transf_comp.scale = glm::vec3{ 5.0f, 5.0f, 5.0f };
	rd.transf_comp.rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };

	rd.render_comp.shader.compile(vs_normals, fs_normals);

	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();

		float angle = (float)window->getTime() * 2;
		rd.transf_comp.angle = angle;

		render_.renderMesh(rd);

		if (window->closePressed()) done = true;
	}

	return 0;
}
