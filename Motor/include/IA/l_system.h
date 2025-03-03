#ifndef __L_SYSTEM__
#define __L_SYSTEM__

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

	void init(std::string& axiom, std::string& rule, int iterations = 50);

private:

	std::string axiom_;
	std::string rule_;

	std::vector<glm::vec3> positions;
	float cube_size = 2.0f;
};

#endif // __L_SYSTEM__