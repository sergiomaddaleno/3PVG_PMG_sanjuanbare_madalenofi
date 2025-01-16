
#include "shader.h"
#include "utils.h"

void Shader::compile(const char* vertex_src, const char* fragment_src)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex_src, nullptr);
	glCompileShader(vertexShader);
	CheckGPUErrors(vertexShader, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", false);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_src, nullptr);
	glCompileShader(fragmentShader);
	CheckGPUErrors(fragmentShader, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", false);

	program_ = glCreateProgram();
	glAttachShader(program_, vertexShader);
	glAttachShader(program_, fragmentShader);
	glLinkProgram(program_);
	CheckGPUErrors(program_, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n", true);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
}

int Shader::get_uniform_location(const char* name) const {

	return glGetUniformLocation(program_, name);
}

void Shader::set_uniform_value(const int uniform_loc, const Type uniform_type, const float* number) const
{
	switch (uniform_type) {

	case T_FLOAT_1: glUniform1f(uniform_loc, number[0]); break;
	case T_FLOAT_2: glUniform2f(uniform_loc, number[0], number[1]); break;
	case T_FLOAT_3: glUniform3f(uniform_loc, number[0], number[1], number[2]); break;
	case T_FLOAT_4: glUniform4f(uniform_loc, number[0], number[1], number[2], number[3]); break;

	case T_MAT_2x2: glUniformMatrix2fv(uniform_loc, 1, GL_FALSE, number); break;
	case T_MAT_3x3: glUniformMatrix3fv(uniform_loc, 1, GL_FALSE, number); break;
	case T_MAT_4x4: glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, number); break;

	}
}

void Shader::useProgram()
{
	glUseProgram(program_);
}

GLuint Shader::getProgram()
{
	return program_;
}
