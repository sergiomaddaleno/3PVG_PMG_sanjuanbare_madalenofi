
#include "window.h"

#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"

#include "ecs_manager.h"
#define M_PI 3.14159265358979323846

static const char* vertex_shader_text =
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

static const char* fragment_shader_text =
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

struct RenderTest {
    std::unique_ptr<Mesh>& render_mesh; 
};

void renderSystem(std::vector<std::optional<RenderTest>>& rc) {
    
    
    for (auto& ri : rc) {
        if (ri.has_value()) {
            
            // Crear y configurar shaders
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertex_shader_text, nullptr);
            glCompileShader(vertexShader);

            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragment_shader_text, nullptr);
            glCompileShader(fragmentShader);

            unsigned int shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            glUseProgram(shaderProgram);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Configurar matrices
            mat4x4 model, view, projection;
            mat4x4_identity(model);

            float angle = (float)glfwGetTime();
            mat4x4_rotate(model, model, 0.0f, 1.0f, 0.0f, angle);



            mat4x4_translate_in_place(model, 0.0f, 0.0f, 0.0f);

            mat4x4_identity(view);


            mat4x4_translate(view, 0.0f, -1.0f, -10.0f);




            mat4x4_perspective(projection, (float)(55.0f * (M_PI / 180.0f)), 800.0f / 600.0f, 0.1f, 100.0f);

            // Enviar matrices al shader
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (const GLfloat*)model);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (const GLfloat*)view);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (const GLfloat*)projection);

            vec3 lightPos = { 1.0f, 1.0f, 1.0f };
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, (const GLfloat*)&lightPos);

            vec3 viewPos = { 1.0f, 1.0f, 3.0f };
            glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, (const GLfloat*)&viewPos);

            // Configurar buffers (VAO, VBO, EBO)
            unsigned int VAO, VBO, EBO;
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, m->vertices.size() * sizeof(Vertex), &m->vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->indices.size() * sizeof(unsigned int), &m->indices[0], GL_STATIC_DRAW);

            // Configurar atributos de v?rtices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
            glEnableVertexAttribArray(2);

            // Dibujar la malla
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, m->indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // Limpiar estado
            glUseProgram(0);

            // Incrementar el ?ndice
        }
        else {
            printf("error\n");
        }
    }
}

int main(int, char**) {

	//---
	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glfwInit();
	//---

    std::unique_ptr<Mesh> mesh_;

    if (!mesh_->LoadOBj("../assets/SM_Teapot.obj")) {
        return -1;
    }

    Render render_;
    auto render = render_.make();
    //---


	ECSManager ecs;

	auto entidad_0 = ecs.create_entity();
	auto entidad_1 = ecs.create_entity();
	ecs.add_component_type<RenderTest>();
    ecs.add_component<RenderTest>(entidad_0)->render_mesh = std::move(mesh_);
    ecs.get_component<RenderTest>(entidad_0);

	ecs.add_component<RenderTest>(entidad_1);
	//ecs.get_component<RenderTest>(entidad_1);
	
	bool done = false;
	while (!done) {

		window->UpdateWindow();

		renderSystem(ecs.getComponentList<RenderTest>());

		if (window->closePressed()) done = true;
	}

	return 0;
}
