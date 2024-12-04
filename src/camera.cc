#define GLM_ENABLE_EXPERIMENTAL
#define PI 3.14159265359
#include "camera.h"
#include <input.h>


#include "../deps/glm/gtx/transform.hpp"

Camera::Camera() : view_direction(0.0f, 0.0f, 0.0f),
position(0.0f, 0.0f, 5.0f),
up(0.0f, 1.0f, 0.0f)
{}

glm::mat4 Camera::initViewTarget()
{
	return glm::lookAt(position, position + view_direction, up);
}



void Camera::update(Input& input, const float window_width, const float window_height, const double dt)
{

    curr_mouse.x = input.getMouseX();
    curr_mouse.y = window_height - input.getMouseY();

    printf("mouse x: %f \n", curr_mouse.x);

    if (input.isMouseButtonPressed(Input::MouseButton::MOUSE_LEFT)) {
        last_mouse = curr_mouse;
    }

    if (input.isMouseButtonDown(Input::MouseButton::MOUSE_LEFT)) {


        delta_mouse = curr_mouse - last_mouse;
        delta_mouse.x = delta_mouse.x / window_width;
        delta_mouse.y = delta_mouse.y / window_height;
        last_mouse = curr_mouse;
        accum_mouse_offset_ += delta_mouse;

        // Limitar la rotación de la cámara en el eje Y
        accum_mouse_offset_.y = glm::clamp(accum_mouse_offset_.y, -90.0f / sensitivity_, 90.0f / sensitivity_);
        
    }

        float alpha = accum_mouse_offset_.x * (PI * 2) * sensitivity_;
        float omega = accum_mouse_offset_.y * (PI - 1.57f) * sensitivity_;


        // Clamp de omega para limitar la rotación en Y de -90 a 90 grados
        omega = glm::clamp(omega, -1.57f, 1.57f);

        view_direction.x = cosf(alpha) * cosf(omega);
        view_direction.y = sinf(omega);
        view_direction.z = sinf(alpha) * cosf(omega);

        if (input.isMouseButtonDown(Input::MouseButton::MOUSE_LEFT)) {
            view = glm::lookAt(position, position + view_direction, up);
        }

        glm::vec3 forward_vec = { view_direction.x, view_direction.y, view_direction.z };
        glm::vec3 up_vec = { 0.0f, 1.0f, 0.0f };

        glm::normalize(forward_vec);
        forward_vec *= speed_ * dt;
  
        glm::vec3 right_vec = glm::cross(forward_vec, up_vec);
    //Movement of camera
   

    if (input.isKeyDown(Input::A)) {
        position = {position.x + (right_vec.x * -1.0f),position.y + (right_vec.y * -1.0f),position.z + (right_vec.z * -1.0f) };
    }

    if (input.isKeyDown(Input::D)) {
        position = { position.x  + right_vec.x,position.y + right_vec.y,position.z + right_vec.z };
    }

    if (input.isKeyDown(Input::W)) {
        position = { position.x + forward_vec.x ,position.y  + forward_vec.y,position.z + forward_vec.z };
    }

    if (input.isKeyDown(Input::S)) {
        position = { position.x + (forward_vec.x * -1.0f) ,position.y  + (forward_vec.y * -1.0f) ,position.z + (forward_vec.z * -1.0f) };
    }

    if (input.isKeyDown(Input::E)) {
        position = { position.x  ,position.y + (speed_ * dt) ,position.z  };
    }
        
    if (input.isKeyDown(Input::Q)) {
        position = { position.x  ,position.y + +(speed_ * dt*-1.0f) ,position.z };
    }
    

}

