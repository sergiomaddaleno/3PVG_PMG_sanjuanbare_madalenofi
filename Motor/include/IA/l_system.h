#ifndef __L_SYSTEM__
#define __L_SYSTEM__

#include <unordered_map>
#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "Core/shader.h"
#include "Geometry/cube.h"

#include "Render/camera.h"

class LSystem {

public:

	LSystem();
	~LSystem();

	//syntax
	//F -> Move forward by line length drawing a line
	//f -> Move forward by line length without drawing a line
	//+ -> Turn left by turning angle
	//- -> Turn right by turning angle
	//[ -> Push current drawing state onto stack
	//] -> Pop current drawing state from the stack

	void init(std::string axiom, std::map<char, std::string> rule_map, int iterations = 1);

	float angle = glm::radians(30.0f);
	float cube_size = 0.5f;
	float spacing = 3.0f;

private:
	glm::vec3 position;
	glm::quat rotation;
};

#endif // __L_SYSTEM__