
#ifndef SHR
#define SHR

#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum Type;

class Shader {

public:

	Shader() : program_(0) {}

	void compile(const char* vertex_src, const char* fragment_src);

	int getUniformLocation(const char* name) const;

	void setUniformValue(const int uniform_loc, const Type uniform_type, const float* number) const;

	void useProgram();

	GLuint getProgram();

private:

	GLuint program_;
};



#endif //SHR