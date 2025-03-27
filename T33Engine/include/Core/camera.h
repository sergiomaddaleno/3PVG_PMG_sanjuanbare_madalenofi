/**
 * @file camera.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __CAMERA__
#define __CAMERA__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Core/input.h"
#include "Core/window.h"
#include <../deps/glm/glm.hpp>

#include <../nlohmann/json.hpp>

 /**
  * @class Camera
  * @brief Represents a 3D camera for scene viewing and navigation.
  */
class Camera {

public:

	Camera();
	~Camera();

	glm::mat4 initViewTarget();

	void update(Input& input, const double dt, const int window_width = 800, const int window_height = 600);

	glm::mat4 getPerspectiveMatrix() const;
	glm::mat4 getOrthograficMatrix() const;

	glm::mat4 getViewMatrix() const;
	void setViewMatrix(const glm::mat4 view);

	// Getters
	const float getSpeed() const {return speed_;}
	const float getSensitivity() const {return sensitivity_;}
	const glm::vec3 getPosition() const{return position_;}
	const float getFov() const{return fov_;}
	float getScrollSpeed() const { return scroll_speed_; }
	glm::vec3 getViewDir() const { return view_direction_; }
	glm::vec3 getUp() const { return up_; }
	const float getNearCliPlane() { return near_clip_plane_; }
	const float getFarCliPlane() { return far_clip_plane_; }
	const bool getUsePerspectiveMatrix() { return use_perspective_matrix_; }

	// Setters
	void setSpeed(const float speed){ speed_ = speed; }
	void setSensitibity(const float sensitivity){ sensitivity_ = sensitivity; }
	void setPosition(glm::vec3 position) {position_ = position;}
	void setFov(float fov) { fov_ = fov; }
	void setAspectRatio(float aspect_ratio) { aspect_ratio_ = aspect_ratio; }
	void setNearCliPlane(float near_clip_plane) { near_clip_plane_ = near_clip_plane; }
	void setFarCliPlane(float far_clip_plane) { far_clip_plane_ = far_clip_plane; }
	void setOrthoWidth(float ortho_width) { ortho_width_ = ortho_width; }
	void setOrthoHeight(float ortho_height) { ortho_height_ = ortho_height; }
	void setUsePerspectiveMatrix(bool use_perspective_matrix) { use_perspective_matrix_ = use_perspective_matrix; }

	void setScrollSpeed(float scroll_speed) { scroll_speed_ = scroll_speed; }
	void setViewDir(glm::vec3 view_direction) {view_direction_ = view_direction;}
	void setUp(glm::vec3 up) { up_ = up; }

	//Save data
	nlohmann::json serialize();
	void deserialize(const nlohmann::json& cam);

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

	float near_clip_plane_;
	float far_clip_plane_;
	float ortho_width_;
	float ortho_height_;

	bool use_perspective_matrix_;
};


#endif // !__CAMERA__