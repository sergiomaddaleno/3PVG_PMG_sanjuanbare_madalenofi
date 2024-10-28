
#include "window.h"
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"        
"layout(location = 1) in vec3 aNormal;\n"    
"layout(location = 2) in vec2 aTexCoord;\n"   
"out vec2 TexCoord;\n"                        
"out vec3 Normal;\n"                          
"out vec3 FragPos;\n"                        
"uniform mat4 model;\n"                       
"uniform mat4 view;\n"                       
"uniform mat4 projection;\n"                  
"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n" 
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n" 
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    TexCoord = aTexCoord;\n"                  
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"                       
"in vec2 TexCoord;\n"                         
"in vec3 Normal;\n"                           
"in vec3 FragPos;\n"                           
"uniform vec3 lightPos;\n"                    
"uniform vec3 viewPos;\n"                      
"void main()\n"
"{\n"
    
"    vec3 norm = normalize(Normal);\n"
    
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
   
"    float diff = max(dot(norm, lightDir), 0.0);\n"
    
"    FragColor = vec4(diff * vec3(1.0, 1.0, 1.0), 1.0); // Blanco\n"
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
