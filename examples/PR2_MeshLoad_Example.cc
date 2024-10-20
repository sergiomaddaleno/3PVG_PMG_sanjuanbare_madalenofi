

#include "window.h"
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"

void RenderMesh(const Mesh& mesh) {

	glBegin(GL_TRIANGLES);
	for (const auto& vertex:mesh.vertices) {
		//glTexCoord2f(vertex.tex_coord[0],vertex.tex_coord[1]);
		//glNormal3f(vertex.normal[0], vertex.normal[1], vertex.normal[2]);
		glVertex3f(vertex.pos[0],vertex.pos[1],vertex.pos[2]);
	}
	glEnd();

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

		RenderMesh(mesh_);
		//r.RenderMesh();

		glfwSwapBuffers(window->GetWindow());
		glfwPollEvents();

		if (window->closePressed()) done = true;

	}

	return 0;
}
