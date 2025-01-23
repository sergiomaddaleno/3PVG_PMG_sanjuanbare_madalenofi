
#define GLM_ENABLE_EXPERIMENTAL
const float PI = 3.141592653589793f;

#include "Render/camera.h"
#include "utils.h"
#include "input.h"
#include "window.h"
#include "global_variables.h"

#include "../deps/glm/gtx/transform.hpp"

Camera::Camera()
    :
    position_(0.0f, 0.0f, 7.0f),
    view_direction_(0.0f, 0.0f, -1.0f),
    up_(0.0f, 1.0f, 0.0f),
    pos_mouse(0.0f, 0.0f),
    curr_mouse(0.0f, 0.0f),
    last_mouse(0.0f, 0.0f),
    delta_mouse(0.0f, 0.0f),
    accum_mouse_offset_(0.0f, 0.0f),
    speed_(0.05f),
    sensitivity_(1.0f),
    fov_(45.0f),
    scroll_speed_(0.5f),
    view_(initViewTarget())
{}


glm::mat4 Camera::initViewTarget()
{
	return glm::lookAt(position_, position_ + view_direction_, up_);
}

void Camera::update(Input& input, const double dt, const int window_width, const int window_height) {
    
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

    view_ = initViewTarget();

    glm::vec3 forward_vec = glm::normalize(glm::vec3(view_direction_.x, view_direction_.y, view_direction_.z));
    forward_vec *= speed_ * dt;

    glm::vec3 right_vec = glm::cross(forward_vec, up_);

    // ----------------- Camera Movement
    if (input.isMouseButtonDown(Input::MouseButton::MOUSE_RIGHT)) {
        if (input.isKeyDown(Input::A)) {
            position_ -= right_vec;
        }
        if (input.isKeyDown(Input::D)) {
            position_ += right_vec;
        }
        if (input.isKeyDown(Input::W)) {
            position_ += forward_vec;
        }
        if (input.isKeyDown(Input::S)) {
            position_ -= forward_vec;
        }
        if (input.isKeyDown(Input::E)) {
            position_.y += speed_ * dt;
        }
        if (input.isKeyDown(Input::Q)) {
            position_.y -= speed_ * dt;
        }
    }

    // ----------------------------------------

    float scroll_delta = input.getScrollY();

    if (scroll_delta > 0.0f) {
        position_ += view_direction_ * scroll_speed_;
    }
    if (scroll_delta < 0.0f) {
        position_ -= view_direction_ * scroll_speed_;
    }

    input.resetScroll();
}

void Camera::setViewMatrix(const glm::mat4 view)
{
    view_ = view;
}

glm::mat4 Camera::getViewMatrix() const {
    return view_;
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(fov_), (float)window_width_ / (float)window_height_, 0.1f, 100.0f);
}
