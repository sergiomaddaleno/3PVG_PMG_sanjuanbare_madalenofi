

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

void RenderMesh(const Mesh& mesh) {

	/*glBegin(GL_TRIANGLES);
	for (const auto& vertex : mesh.vertices) {
		//glTexCoord2f(vertex.tex_coord[0],vertex.tex_coord[1]);
		//glNormal3f(vertex.normal[0], vertex.normal[1], vertex.normal[2]);
		glVertex3f(vertex.pos[0], vertex.pos[1], vertex.pos[2]);
	}
	glEnd();
	*/
}

int main(int, char**) {

	Mesh mesh_;

	if (!mesh_.LoadOBj("../assets/SM_Teapot.obj")) {
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

		//RenderMesh(mesh_);
		r.RenderMesh(vertex_shader_text,fragment_shader_text,mesh_);

		glfwSwapBuffers(window->GetWindow());
		glfwPollEvents();

		if (window->closePressed()) done = true;

	}

	return 0;
}
