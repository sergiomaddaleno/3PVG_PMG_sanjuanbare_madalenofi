
#include "Core/shader.h"
#include "utils.h"

Shader::Shader() : program_(0) {}

Shader::~Shader()
{
}

void Shader::compile(const char* vertex_src, const char* fragment_src)
{
    if (program_ != 0) {
        glUseProgram(0);
        glDeleteProgram(program_); 
        program_ = 0;
    }
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_src, nullptr);
    glCompileShader(vertexShader);
    //CheckGPUErrors(vertexShader, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", false);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_src, nullptr);
    glCompileShader(fragmentShader);
    //CheckGPUErrors(fragmentShader, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", false);

   
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);
    //CheckGPUErrors(program_, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n", true);

   
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);
}

bool Shader::isCompiled() const
{
	GLint success;
	glGetProgramiv(program_, GL_LINK_STATUS, &success);
	return success == GL_TRUE;
}

int Shader::getUniformLocation(const char* name) const {

	return glGetUniformLocation(program_, name);
}

void Shader::setUniformValue(const int uniform_loc, const ValueType uniform_type, const void* data) const
{
	switch (uniform_type) {

	case T_FLOAT_1: glUniform1f(uniform_loc, *((float*)data)); break;
	case T_FLOAT_2: glUniform2f(uniform_loc, ((float*)data)[0], ((float*)data)[1]); break;
	case T_FLOAT_3: glUniform3f(uniform_loc, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2]); break;
	case T_FLOAT_4: glUniform4f(uniform_loc, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2], ((float*)data)[3]); break;

	case T_INT_1: glUniform1i(uniform_loc, *((int*)data)); break;
	case T_INT_2: glUniform2i(uniform_loc, ((int*)data)[0], ((int*)data)[1]); break;
	case T_INT_3: glUniform3i(uniform_loc, ((int*)data)[0], ((int*)data)[1], ((int*)data)[2]); break;
	case T_INT_4: glUniform4i(uniform_loc, ((int*)data)[0], ((int*)data)[1], ((int*)data)[2], ((int*)data)[3]); break;

	case T_MAT_2x2: glUniformMatrix2fv(uniform_loc, 1, GL_FALSE, (float*)data); break;
	case T_MAT_3x3: glUniformMatrix3fv(uniform_loc, 1, GL_FALSE, (float*)data); break;
	case T_MAT_4x4: glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (float*)data); break;

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

nlohmann::json Shader::serialize() const
{
    nlohmann::json json_data;

    json_data["Program"] = program_;

    return json_data;
}
