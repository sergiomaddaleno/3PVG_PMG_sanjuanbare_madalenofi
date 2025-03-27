#ifndef __CAMERA_COMPONENT__
#define __CAMERA_COMPONENT__

#include "Core/camera.h"
#include "../deps/nlohmann/json.hpp"

struct CameraComponent {

    Camera camera;

    float window_width=800.0f;
    float window_height=600.0f;

    // ---

    nlohmann::json serialize();

    CameraComponent();
    ~CameraComponent();
};

#endif // __CAMERA_COMPONENT__
