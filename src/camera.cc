
#define GLM_ENABLE_EXPERIMENTAL
const float PI = 3.141592653589793f;

#include "camera.h"
#include <input.h>
#include "../deps/glm/gtx/transform.hpp"

Camera::Camera() : 

position_(0.0f, 0.0f, 7.0f),
view_direction_(0.0f, 0.0f, -1.0f),
up_(0.0f, 1.0f, 0.0f),
pos_mouse(0.0f, 0.0f),
curr_mouse(0.0f, 0.0f),
last_mouse(0.0f, 0.0f),
delta_mouse(0.0f, 0.0f),
accum_mouse_offset_(0.0f, 0.0f),
speed_(0.001f),
sensitivity_(1.0f)

{
    view_ = initViewTarget();
}

glm::mat4 Camera::initViewTarget()
{
	return glm::lookAt(position_, position_ + view_direction_, up_);
}

void Camera::update(Input& input, const double dt, const int window_width, const int window_height){

    // ----------------- Camera Rotation

    curr_mouse.x = (float)input.getMouseX();
    curr_mouse.y = window_height - (float)input.getMouseY();

    if (input.isMouseButtonPressed(Input::MouseButton::MOUSE_RIGHT)) {
        last_mouse = curr_mouse;
    }

    if (input.isMouseButtonDown(Input::MouseButton::MOUSE_RIGHT)) {

        delta_mouse = curr_mouse - last_mouse;
        delta_mouse.x = delta_mouse.x / window_width;
        delta_mouse.y = delta_mouse.y / window_height;
        last_mouse = curr_mouse;
        accum_mouse_offset_ += delta_mouse;

        accum_mouse_offset_.y = glm::clamp(accum_mouse_offset_.y, -90.0f / sensitivity_, 90.0f / sensitivity_);
    }

    float alpha = accum_mouse_offset_.x * (PI * 2) * sensitivity_;
    float omega = accum_mouse_offset_.y * (PI - 1.57f) * sensitivity_;

    omega = glm::clamp((float)omega, -1.57f, 1.57f);

    view_direction_.x = cosf(alpha) * cosf(omega);
    view_direction_.y = sinf(omega);
    view_direction_.z = sinf(alpha) * cosf(omega);

    if (input.isMouseButtonDown(Input::MouseButton::MOUSE_RIGHT)) {
        view_ = initViewTarget();
    }

    glm::vec3 forward_vec = { view_direction_.x, view_direction_.y, view_direction_.z };

    glm::normalize(forward_vec);
    forward_vec *= speed_ * dt;
  
    glm::vec3 right_vec = glm::cross(forward_vec, up_);

    // ----------------- Camera Movement

    if (input.isMouseButtonDown(Input::MouseButton::MOUSE_RIGHT)) {
        if (input.isKeyDown(Input::A)) {
            position_ = { position_.x + (right_vec.x * -1.0f),position_.y + (right_vec.y * -1.0f),position_.z + (right_vec.z * -1.0f) };
        }

        if (input.isKeyDown(Input::D)) {
            position_ = { position_.x + right_vec.x,position_.y + right_vec.y,position_.z + right_vec.z };
        }

        if (input.isKeyDown(Input::W)) {
            position_ = { position_.x + forward_vec.x ,position_.y + forward_vec.y,position_.z + forward_vec.z };
        }

        if (input.isKeyDown(Input::S)) {
            position_ = { position_.x + (forward_vec.x * -1.0f) ,position_.y + (forward_vec.y * -1.0f) ,position_.z + (forward_vec.z * -1.0f) };
        }

        if (input.isKeyDown(Input::E)) {
            position_ = { position_.x  ,position_.y + (speed_ * dt) ,position_.z };
        }

        if (input.isKeyDown(Input::Q)) {
            position_ = { position_.x  ,position_.y + +(speed_ * dt * -1.0f) ,position_.z };
        }
    }
    
    // ----------------- Camera speed

    if (input.getScrollY() > 0.00f && input.isKeyDown(Input::SHIFT)) {
        speed_ += 0.001f;
    }

    if (input.getScrollY() < 0.00f && input.isKeyDown(Input::SHIFT)) {
        speed_ -= 0.001f;
    }

    if (speed_ <= 0.001f) speed_ = 0.001f;
    if (speed_ >= 0.15f) speed_ = 0.15f;
}

void Camera::setSpeed(const float speed) {
    speed_ = speed;
}

void Camera::setSensitibity(const float sensitivity) {
    sensitivity_ = sensitivity;
}

void Camera::setViewMatrix(const glm::mat4 view)
{
    view_ = view;
}

const float Camera::getSpeed() const {
    return speed_;
}

const float Camera::getSensitivity() const {
    return sensitivity_;
}

glm::mat4 Camera::getViewMatrix(){
    return view_;
}