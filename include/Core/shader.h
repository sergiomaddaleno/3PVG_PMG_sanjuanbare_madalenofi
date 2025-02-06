
#ifndef SHR
#define SHR

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../deps/nlohmann/json.hpp"

enum ValueType {
    T_NONE = 0x0,
    T_FLOAT_1,
    T_FLOAT_2,
    T_FLOAT_3,
    T_FLOAT_4,

    T_INT_1,
    T_INT_2,
    T_INT_3,
    T_INT_4,

    T_MAT_4x4,
    T_MAT_3x3,
    T_MAT_2x2,

    T_SAMPLER_1D,
    T_SAMPLER_2D,
    T_SAMPLER_3D,
};

class Shader {

public:

	Shader() : program_(0) {}

	void compile(const char* vertex_src, const char* fragment_src);

	bool isCompiled() const;

	int getUniformLocation(const char* name) const;

	void setUniformValue(const int uniform_loc, const ValueType uniform_type, const void* data) const;

	void useProgram();

	GLuint getProgram();

    //Save data
    nlohmann::json serialize() const;

private:

	GLuint program_;
};



#endif //SHR