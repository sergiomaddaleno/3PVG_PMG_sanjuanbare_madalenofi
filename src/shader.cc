
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

void Shader::useProgram()
{
	glUseProgram(program_);
}

GLuint Shader::getProgram()
{
	return program_;
}
