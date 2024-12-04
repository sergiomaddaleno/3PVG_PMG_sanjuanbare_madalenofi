
#ifndef CAM
#define CAM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "..\include\input.h"
#include <../deps/glm/glm.hpp>

class Camera {

public:

	glm::vec3 position;
	glm::vec3 view_direction;
	glm::vec3 up;

	glm::vec2  vertex_camera = { 0.0f, 0.0f };
	glm::vec2 curr_mouse = { 0.0f, 0.0f };
	glm::vec2 last_mouse = { 0.0f, 0.0f };
	glm::vec2 delta_mouse = { 0.0f, 0.0f };

	bool enabled_;
	float speed_ = 0.01f;
	float sensitivity_=1.0f;
	glm::vec2 accum_mouse_offset_={ 0.0f, 0.0f };;
	
	glm::mat4 view;
	glm::vec2 pos_mouse;

	Camera();

	glm::mat4 initViewTarget();

	void update(Input& input, const float window_width, const float window_height, const double dt);

};


#endif // PR0