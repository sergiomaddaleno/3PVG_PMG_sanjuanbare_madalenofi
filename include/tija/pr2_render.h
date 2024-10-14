
#ifndef PRR
#define PRR

#include <optional>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tija/PR1_Triangle.h"
#include <tija/linmath.h>

class Render {

public:
	 
	GLuint program_;
	GLint mvp_location_;

	Render(GLuint program, GLint mvp_location);

	static std::optional<Render> make(static const char* vertex_shader_text, 
		static const char* fragment_shader_text, Triangle::PointsTriangle* vertices);

	GLuint GetProgram();

	GLint GetLocation();
private:
	 
	//static GLint mvp_location_;

};

#endif // PRT