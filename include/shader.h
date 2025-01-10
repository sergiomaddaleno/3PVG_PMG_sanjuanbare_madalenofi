
#ifndef SHR
#define SHR

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Shader {

public:

	Shader() : program_(0) {}

	void compile(const char* vertex_src, const char* fragment_src);

	void useProgram();

	GLuint getProgram();

private:

	GLuint program_;
};



#endif //SHR