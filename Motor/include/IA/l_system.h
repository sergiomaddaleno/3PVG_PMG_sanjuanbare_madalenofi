#ifndef __L_SYSTEM__
#define __L_SYSTEM__

#include <unordered_map>
#include <../deps/glm/glm.hpp>

#include "Core/shader.h"
#include "Geometry/cube.h"

#include "Render/camera.h"

class LSystem {

public:

	LSystem();
	~LSystem();

	struct Branch {
		glm::vec3 position;
	};

	//syntax
	// C -> Create cube
	// E -> Create Empty
	// + -> Go up in y
	// - -> Go down in y
	// R -> Go right in x
	// L -> Go left in x

	void init(std::string axiom, std::string rules, int iterations = 5);

private:

	std::vector<glm::vec3> positions;
	float cube_size = 2.0f;
};

#endif // __L_SYSTEM__