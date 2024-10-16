
#ifndef PRR
#define PRR

#include <optional>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "triangle.h"
#include <../deps/linmath.h>

class Render {

public:
	 
	GLuint program_;
	GLint mvp_location_;

	Render(GLuint program, GLint mvp_location);

	static std::optional<Render> make(static const char* vertex_shader_text, 
		static const char* fragment_shader_text, Triangle::PointsTriangle(&vertices)[3]);

	GLuint GetProgram();
	GLint GetLocation();
};

#endif // PRT