
#define TINYOBJLOADER_IMPLEMENTATION
#include "window.h"


#include <optional>
#include "..\include\mesh.h"
#include <string>


void glfwErrorCallback(int error, const char* description) {
	printf("GLFW Error (%d): %s\n", error, description);
}

void RenderMesh(const Mesh& mesh) {

	glBegin(GL_TRIANGLES);
	for (const auto& vertex:mesh.vertices) {
		glTexCoord2f(vertex.tex_coord[0],vertex.tex_coord[1]);
		glNormal3f(vertex.normal[0], vertex.normal[1], vertex.normal[2]);
		glVertex3f(vertex.pos[0],vertex.pos[1],vertex.pos[2]);

	}
	glEnd();

}



int main(int, char**) {

	glfwSetErrorCallback(glfwErrorCallback);

	//---
	
	
	Mesh mesh_;

	if (!mesh_.LoadOBj("../assets/SM_Teapot.obj")) {
		return -1;
	}
	
	glfwInit();

	

	auto window = Window::make(800, 600, "Window");
	window->SetWindowActive();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//---

	bool done = false;
	while (!done) {

		RenderMesh(mesh_);
		glfwSwapBuffers(window->GetWindow());
		
		glfwPollEvents();
		
		bool close_pressed = window->closePressed();
		if (close_pressed) done = true;

	}

	return 0;
}
