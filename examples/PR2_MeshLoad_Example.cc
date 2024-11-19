
#include "window.h"
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"

static const char* vertex_shader_text_normals =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"out vec3 normal;\n"
"out vec3 FragPos;\n"
"out vec3 pos;\n"
"out vec2 uv;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"    normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    pos = aPos;\n"
"    uv = aTexCoord;\n"
"    FragPos = aPos;\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

static const char* fragment_shader_text_normals =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 normal;\n"
"in vec3 FragPos;\n"
"in vec3 pos;\n"
"in vec2 uv;\n"
"void main() {\n"
"    vec3 norm = normalize(normal);\n"
"    FragColor = vec4(norm, 1.0);\n"
"}\n";

int main(int, char**) {

	std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();

	if (!mesh_->LoadOBj("../assets/SM_Teapot.obj")) {
		return -1;
	}
	
	glfwInit();

	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//---

	Render r;
	auto render = r.make();

	bool done = false;
	while (!done) {

		r.RenderMesh(vertex_shader_text,fragment_shader_text,mesh_);

		window->UpdateWindow();

		if (window->closePressed()) done = true;
	}

	return 0;
}
