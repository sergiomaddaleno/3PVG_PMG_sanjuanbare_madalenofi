
#ifndef SHR
#define SHR

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../deps/nlohmann/json.hpp"

enum Type;

class Shader {

public:

	Shader() : program_(0) {}

	void compile(const char* vertex_src, const char* fragment_src);

	bool isCompiled() const;

	int getUniformLocation(const char* name) const;

	void setUniformValue(const int uniform_loc, const Type uniform_type, const void* data) const;

	void useProgram();

	GLuint getProgram();

    //Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        json_data["program"] = program_;

        return json_data;
    }

private:

	GLuint program_;
};



#endif //SHR