
#define GLM_ENABLE_EXPERIMENTAL
const float PI = 3.141592653589793f;

#include "Core/camera.h"
#include "utils.h"
#include "Core/input.h"
#include "Core/window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../nlohmann/json.hpp>
#include "../deps/glm/gtx/transform.hpp"

Camera::Camera()
    :
    position_(0.0f, 5.0f, 10.0f),
    view_direction_(0.0f, 0.0f, 1.0f),
    up_(0.0f, 1.0f, 0.0f),
    pos_mouse(0.0f, 0.0f),
    curr_mouse(0.0f, 0.0f),
    last_mouse(0.0f, 0.0f),
    delta_mouse(0.0f, 0.0f),
    accum_mouse_offset_(0.0f, 0.0f),
    speed_(3.0f),
    sensitivity_(1.0f),
    fov_(45.0f),
    aspect_ratio_(800.0f / 600.0f),
    near_clip_plane_(0.1f),
    far_clip_plane_(200.0f),
    ortho_width_(45.0f),
    ortho_height_(45.0f),
    scroll_speed_(1.0f),
    use_perspective_matrix_(true),
    lock_view_dir_(false),
    lock_move_(false),
    lock_wheel_mouse_(false),
    view_(initViewTarget())
{}

Camera::~Camera()
{
}

glm::mat4 Camera::initViewTarget()
{
	return glm::lookAt(position_, position_ + view_direction_, up_);
}

glm::mat4 Camera::getPerspectiveMatrix() const
{
    return glm::perspective(glm::radians(fov_), aspect_ratio_, near_clip_plane_, far_clip_plane_);
}

glm::mat4 Camera::getOrthograficMatrix() const
{
    float w_half = ortho_width_ / 2.0f;
    float h_half = ortho_height_ / 2.0f;

    return glm::ortho(-w_half, w_half, -h_half, h_half, near_clip_plane_, far_clip_plane_);
}

void Camera::update(Input& input, const double dt, const int window_width, const int window_height) {

    // ----------------- Camera Rotation
    curr_mouse.x = (float)input.getMouseX();
    curr_mouse.y = window_height - (float)input.getMouseY();

    if (input.isMouseButtonPressed(Input::MouseButton::MOUSE_RIGHT)) {
        last_mouse = curr_mouse;
    }

    if (!lock_view_dir_ && input.isMouseButtonDown(Input::MouseButton::MOUSE_RIGHT)) {
        delta_mouse = curr_mouse - last_mouse;
        delta_mouse.x = delta_mouse.x / window_width;
        delta_mouse.y = delta_mouse.y / window_height;
        last_mouse = curr_mouse;
        accum_mouse_offset_ += delta_mouse;

        accum_mouse_offset_.y = glm::clamp(accum_mouse_offset_.y, -90.0f / sensitivity_, 90.0f / sensitivity_);

        float alpha = accum_mouse_offset_.x * (PI * 2) * sensitivity_;
        float omega = accum_mouse_offset_.y * (PI - 1.57f) * sensitivity_;

        omega = glm::clamp((float)omega, -1.57f, 1.57f);

        view_direction_.x = cosf(alpha) * cosf(omega);
        view_direction_.y = sinf(omega);
        view_direction_.z = sinf(alpha) * cosf(omega);
    }

    view_ = initViewTarget();

    glm::vec3 forward_vec = glm::normalize(view_direction_);
    glm::vec3 right_vec = glm::normalize(glm::cross(forward_vec, up_));

    // ----------------- Camera Movement
    if (!lock_move_ && input.isMouseButtonDown(Input::MouseButton::MOUSE_RIGHT)) {
        if (input.isKeyDown(Input::A)) position_ -= right_vec * speed_ * (float)dt;
        if (input.isKeyDown(Input::D)) position_ += right_vec * speed_ * (float)dt;
        if (input.isKeyDown(Input::W)) position_ += forward_vec * speed_ * (float)dt;
        if (input.isKeyDown(Input::S)) position_ -= forward_vec * speed_ * (float)dt;
        if (input.isKeyDown(Input::E)) position_.y += speed_ * (float)dt;
        if (input.isKeyDown(Input::Q)) position_.y -= speed_ * (float)dt;
    }

    // ----------------------------------------

    if (!lock_wheel_mouse_) {
        float scroll_delta = (float)input.getScrollY();

        if (use_perspective_matrix_) {
            if (scroll_delta > 0.0f) {
                position_ += view_direction_ * scroll_speed_;
            }
            if (scroll_delta < 0.0f) {
                position_ -= view_direction_ * scroll_speed_;
            }
        }
        else {
            if (scroll_delta > 0.0f) {
                ortho_width_ *= 0.9f;
                ortho_height_ *= 0.9f;
            }
            if (scroll_delta < 0.0f) {
                ortho_width_ *= 1.1f;
                ortho_height_ *= 1.1f;
            }
        }

        input.resetScroll();
    }
}

void Camera::setViewMatrix(const glm::mat4 view)
{
    view_ = view;
}

glm::quat Camera::getOrientation() const
{
    return glm::quatLookAt(view_direction_, up_);
}

glm::mat4 Camera::getViewMatrix() const {
    return view_;
}

void Camera::setOrientation(glm::quat orientation)
{
    view_direction_ = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    up_ = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    view_ = initViewTarget();
}

nlohmann::json Camera::serialize()
{
    nlohmann::json json_data;

    json_data["Position"] = {position_.x, position_.y, position_.z};
    json_data["View_direction"] = { view_direction_.x, view_direction_.y, view_direction_.z };
    json_data["Speed"] = speed_;
    json_data["Sensitivity"] = sensitivity_;
    json_data["Scroll_speed"] = scroll_speed_;
    json_data["Fov"] = fov_;
    json_data["Aspect_ratio"] = aspect_ratio_;
    json_data["Near_clip_plane"] = near_clip_plane_;
    json_data["Far_clip_plane"] = far_clip_plane_;
    json_data["Ortho_width"] = ortho_width_;
    json_data["Ortho_height"] = ortho_height_;
    json_data["Use_perspective_matrix"] = use_perspective_matrix_;
    json_data["Lock_view_dir"] = lock_view_dir_;
    json_data["Lock_move"] = lock_move_;
    json_data["Lock_wheel_mouse"] = lock_wheel_mouse_;

    return json_data;
}

void Camera::deserialize(const nlohmann::json& cam)
{
    this->setPosition(parseVec3(cam["Position"]));
    this->setFov(cam["Fov"]);
    this->setScrollSpeed(cam["Scroll_speed"]);
    this->setSensitibity(cam["Sensitivity"]);
    this->setSpeed(cam["Speed"]);
    this->setAspectRatio(cam["Aspect_ratio"]);
    this->setNearCliPlane(cam["Near_clip_plane"]);
    this->setFarCliPlane(cam["Far_clip_plane"]);
    this->setNearCliPlane(cam["Near_clip_plane"]);
    this->setFarCliPlane(cam["Far_clip_plane"]);
    this->setOrthoWidth(cam["Ortho_width"]);
    this->setOrthoHeight(cam["Ortho_height"]);
    this->setUsePerspectiveMatrix(cam["Use_perspective_matrix"]);
    this->setViewDir(parseVec3(cam["View_direction"]));
    this->initViewTarget();
    this->lockViewDirection(cam["Lock_view_dir"]);
    this->lockMovement(cam["Lock_move"]);
    this->lockWheel(cam["Lock_wheel_mouse"]);
}
