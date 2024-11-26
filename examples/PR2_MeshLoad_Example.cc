
#include "window.h"
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\Components\transform_component.h"

std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();

Render render_;
ShaderBuffers shaders = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders);

int main(int, char**) {

	if (!mesh_->LoadOBj("../assets/obj/SM_Teapot.obj")) {
		return -1;
	}
	
	//---
	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();
	//---

	auto render = render_.make();

	TransformComponent tr;
	tr.position = Vec3{ 0.0f, -1.0f, -5.0f };
	tr.scale = Vec3{ 6.0f, 6.0f, 6.0f };
	tr.rotation = Vec3{ 0.0f, 1.0f, 0.0f };

	bool done = false;
	while (!done) {

		float angle = (float)glfwGetTime() * 2;
		tr.angle = angle;

		render_.SetupShader(shaders);
		render_.RenderMesh(mesh_, tr);

		window->UpdateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
