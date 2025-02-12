#ifndef __CAMERA__
#define __CAMERA__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Core/input.h"
#include "Core/window.h"
#include <../deps/glm/glm.hpp>

#include <../nlohmann/json.hpp>

class Camera {

public:

	Camera();

	glm::mat4 initViewTarget();

	void update(Input& input, const double dt, const int window_width = 800, const int window_height = 600);

	// ---

	const float getSpeed() const {return speed_;}
	const float getSensitivity() const {return sensitivity_;}
	const glm::vec3 getPosition() const{return position_;}
	const float getFov() const{return fov_;}

	float getScrollSpeed() const { return scroll_speed_; }
	glm::vec3 getViewDir() const { return view_direction_; }
	glm::vec3 getUp() const { return up_; }

	//--
	void setSpeed(const float speed){ speed_ = speed; }
	void setSensitibity(const float sensitivity){ sensitivity_ = sensitivity; }
	void setPosition(glm::vec3 position) {position_ = position;}
	void setFov(float fov) { fov_ = fov; }
	void setAspectRatio(float aspect_ratio) { aspect_ratio_ = aspect_ratio; }
	void setMinRenderDistance_(float min_render_distance) { min_render_distance_ = min_render_distance; }
	void setMaxRenderDistance_(float max_render_distance) { max_render_distance_ = max_render_distance; }

	void setScrollSpeed(float scroll_speed) { scroll_speed_ = scroll_speed; }
	void setViewDir(glm::vec3 view_direction) {view_direction_ = view_direction;}
	void setUp(glm::vec3 up) { up_ = up; }

	//--
	glm::mat4 getViewMatrix() const;
	void setViewMatrix(const glm::mat4 view);

	glm::mat4 getProjectionMatrix() const;

	//Save data
	nlohmann::json serialize();

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
	float fov_;
	float aspect_ratio_;
	float min_render_distance_;
	float max_render_distance_;
};


#endif // __CAMERA__