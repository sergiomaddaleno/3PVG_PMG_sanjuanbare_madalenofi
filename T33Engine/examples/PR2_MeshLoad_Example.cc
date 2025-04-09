
#include <optional>
#include <string>

#include "Core/window.h"
#include "Core/mesh.h"
#include "Core/shader.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"
#include "Render/render_system.h"
#include "Core/input.h"
#include "utils.h"

#include <../deps/glm/glm.hpp>

auto mesh_ = std::make_shared<Mesh>();
RenderComponent rc;
TransformComponent tc;
Shader shader;

int main(int, char**) {
	
	//---
	auto window = Window::make(800, 600, "Window");
	window->setWindowActive();
	window->initGLEW();
	//---

	Input input{ window->getWindow() };

	if (!mesh_->loadOBJ("../stages/assets/obj/SM_Teapot.obj")) {
		return -1;
	}

	rc.mesh_ = std::move(mesh_);
	tc.position = glm::vec3{ 0.0f, 0.0f, -5.0f };
	tc.scale = glm::vec3{ 5.0f, 5.0f, 5.0f };

	initShader(shader, "../stages/shaders/basic/normals.vs", "../stages/shaders/basic/normals.fs");

	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();

		float time = (float)window->getTime() * 2;
		tc.rotation = glm::vec3{ 0.0f, time, 0.0f };

		rc.mesh_->basicRender(shader, rc, tc);

		if (window->closePressed(input)) done = true;
	}

	return 0;
}
