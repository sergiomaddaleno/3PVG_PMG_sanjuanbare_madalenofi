
#ifndef CAM
#define CAM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "..\include\input.h"
#include <../deps/glm/glm.hpp>

class Camera {

public:

	Camera();

	glm::mat4 initViewTarget();

	void update(Input& input, const float window_width, const float window_height, const double dt);

	// ---

	void setSpeed(const float speed);

	void setSensitibity(const float sensitivity);

	void setViewMatrix(const glm::mat4 view);

	const float getSpeed() const;

	const float getSensitivity() const;

	glm::mat4 getViewMatrix();

private:

	glm::vec3 position_;
	glm::vec3 view_direction_;
	glm::vec3 up_;

	glm::mat4 view_;

	glm::vec2 pos_mouse;
	glm::vec2 curr_mouse;
	glm::vec2 last_mouse;
	glm::vec2 delta_mouse;
	glm::vec2 accum_mouse_offset_;

	float speed_;
	float sensitivity_;
};


#endif // CAM