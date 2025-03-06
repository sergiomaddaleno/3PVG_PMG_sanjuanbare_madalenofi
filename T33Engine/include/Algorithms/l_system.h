#ifndef __L_SYSTEM__
#define __L_SYSTEM__

#include <unordered_map>

#include "Core/shader.h"
#include "Geometry/cube.h"
#include "Render/render_system.h"
#include "../Systems/ecs_manager.h"

class LSystem {

public:

	LSystem(ECSManager &ecs, RenderSystem& rs);
	~LSystem();

	//syntax
	// F -> Move forward by line length drawing a line
	// f -> Move forward by line length without drawing a line
	// + -> Turn left by turning angle
	// - -> Turn right by turning angle
	// [ -> Push current drawing state onto stack
	// ] -> Pop current drawing state from the stack

	//Example usage
	//std::map<char, std::string> rules = { {'F', "F[-F][+F]"} };
	//std::map<char, std::string> rules = { {'F', "FF+[+F-F-F]-[-F+F+F]"} };
	//lsystem.init("F", rules, 2);

	void init(std::string axiom, std::map<char, std::string> rule_map, int iterations = 1);

	float angle_;
	glm::vec3 cube_size_;
	float spacing_;
	glm::vec3 starting_pos_;
	glm::quat starting_rot_;

private:
	glm::vec3 position_;
	glm::quat rotation_;

	ECSManager& ecs_;
	RenderSystem& rs_;
};

#endif // __L_SYSTEM__