
#include "..\include\mesh.h"
#include "..\include\render.h"

#include "window.h"
#include <optional>
#include <string>

#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

#include <../deps/glm/glm.hpp>

std::shared_ptr<Mesh> mesh_ = std::make_shared<Mesh>();

Render render_;
ShaderBuffers shaders = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders);

int main(int, char**) {
	
	//---
	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	//---

	auto render = render_.make();

	if (!mesh_->loadOBJ("../assets/obj/SM_Teapot.obj")) {
		return -1;
	}

	TransformComponent tr;
	tr.position = glm::vec3{ 0.0f, -2.0f, -5.0f };
	tr.scale = glm::vec3{ 3.0f, 3.0f, 3.0f };
	tr.rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };

	RenderComponent rc;
	rc.mesh_ = mesh_;

	bool done = false;
	while (!done) {

		float angle = (float)glfwGetTime() * 2;
		tr.angle = angle;

		render_.setupShader(shaders);
		render_.renderMesh(rc, tr);

		window->updateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
