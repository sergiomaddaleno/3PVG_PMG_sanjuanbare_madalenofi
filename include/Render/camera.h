
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

	void update(Input& input, const double dt, const int window_width = 800, const int window_height = 600);

	// ---

	void setSpeed(const float speed);

	void setSensitibity(const float sensitivity);

	void setViewMatrix(const glm::mat4 view);

	const float getSpeed() const;

	const glm::vec3 getPosition() const;

	const float getSensitivity() const;

	glm::mat4 getViewMatrix() const;

	glm::mat4 getProjectionMatrix() const;

	//--
	void setPosition(glm::vec3 position) {position = position_;}
	void setScrollSpeed(float scroll_speed) { scroll_speed = scroll_speed_; }
	void setViewDir(glm::vec3 view_direction) {view_direction = view_direction_;}
	void setUp(glm::vec3 up) { up = up_; }

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
	float scroll_speed_;
};


#endif // CAM