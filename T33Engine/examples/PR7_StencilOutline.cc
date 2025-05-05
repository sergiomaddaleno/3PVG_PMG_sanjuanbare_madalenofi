
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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

auto mesh_1_ = std::make_shared<Mesh>();
auto mesh_2_ = std::make_shared<Mesh>();

TransformComponent tc;
Shader shader, stencil_outline;
Camera cam;

float outline_scale = 1.05f;
glm::vec4 outline_color = { 1.0f, 1.0f, 1.0f, 1.0f };

void render(Shader shader, Mesh& mesh) {

	shader.useProgram();

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
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

void renderOutline(Shader shader, Mesh& mesh) {
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	shader.useProgram();

	for (auto& submesh : mesh.meshes) {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), tc.position);
		glm::mat4 rotation = glm::mat4_cast(tc.rotation);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), tc.scale);

		glm::mat4 model = translation * rotation * scale;
		glm::mat4 view = cam.getViewMatrix();
		glm::mat4 projection = cam.getPerspectiveMatrix();

		shader.setUniformValue(shader.getUniformLocation("model"), T_MAT_4x4, glm::value_ptr(model));
		shader.setUniformValue(shader.getUniformLocation("view"), T_MAT_4x4, glm::value_ptr(view));
		shader.setUniformValue(shader.getUniformLocation("projection"), T_MAT_4x4, glm::value_ptr(projection));

		shader.setUniformValue(shader.getUniformLocation("u_outline_scale"), T_FLOAT_1, &outline_scale);
		shader.setUniformValue(shader.getUniformLocation("u_outline_color"), T_FLOAT_4, &outline_color);

		mesh.draw();
	}

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
}

void MyImGui() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImVec2(300, 120), ImGuiCond_FirstUseEver);
	ImGui::Begin("Menu");

	ImGui::SliderFloat("Outline scale", &outline_scale, 1.004f, 1.194f, "%.3f");
	ImGui::ColorEdit4("Outline color", glm::value_ptr(outline_color));

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void InitImGui(GLFWwindow* w) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	int width, height;
	glfwGetFramebufferSize(w, &width, &height);
	io.DisplaySize = ImVec2((float)width, (float)height);

	ImGui_ImplGlfw_InitForOpenGL(w, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

int main(int, char**) {

	//---
	auto window = Window::make(1280, 1024, "Window");
	window->setWindowActive();
	window->initGLEW();
	//---

	Input input{ window->getWindow() };
	InitImGui(window->getWindow());

	mesh_1_->loadOBJ("../stages/assets/obj/LMcQueen.obj");
	initShader(shader, "../stages/shaders/basic/normals.vs", "../stages/shaders/basic/normals.fs");
	initShader(stencil_outline, "../stages/shaders/outlining/outline.vs", "../stages/shaders/outlining/outline.fs");

	mesh_1_ = std::move(mesh_1_);
	tc.position = glm::vec3{ 0.0f, 1.0f, -3.0f };

	mesh_1_->bind();

	cam.setViewDir(glm::vec3(0, 0, -1.0));
	cam.setSpeed(0.005f);
	cam.setSensitibity(0.5f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	double lastTime = window->getTime();
	bool done = false;
	while (!done) {
		double currentTime = window->getTime();
		float dt = float(currentTime - lastTime);
		window->updateWindow();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		cam.update(input, dt);

		float time = (float)window->getTime() * 2;
		tc.rotation = glm::vec3{ 0.0f, time, 0.0f };

		MyImGui();

		render(shader, *mesh_1_);
		renderOutline(stencil_outline, *mesh_1_);

		if (window->closePressed(input)) done = true;
	}

	return 0;
}
