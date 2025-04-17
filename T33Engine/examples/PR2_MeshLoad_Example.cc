
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
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

auto mesh_ = std::make_shared<Mesh>();
TransformComponent tc;
Shader shader;
Camera cam;

void render(Shader shader, Mesh& mesh) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	shader.useProgram();

	for (auto& submesh : mesh.meshes) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, tc.position);
		model = model * glm::mat4_cast(tc.rotation);
		model = glm::scale(model, tc.scale);

		glm::mat4 view = cam.getViewMatrix();
		glm::mat4 projection = cam.getPerspectiveMatrix();

		shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
		shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
		shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

		mesh.draw();
	}
}

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
	initShader(shader, "../stages/shaders/basic/normals.vs", "../stages/shaders/basic/normals.fs");

	mesh_ = std::move(mesh_);
	tc.position = glm::vec3{ 0.0f, 1.0f, -3.0f };
	tc.scale = glm::vec3{ 5.0f, 5.0f, 5.0f };

	mesh_->bind();

	cam.setViewDir(glm::vec3(0, 0, -1.0));
	cam.lockViewDirection(true);
	cam.lockMovement(true);

	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();

		cam.update(input, dt);

		float time = (float)window->getTime() * 2;
		tc.rotation = glm::vec3{ 0.0f, time, 0.0f };

		render(shader, *mesh_);

		if (window->closePressed(input)) done = true;
	}

	return 0;
}
