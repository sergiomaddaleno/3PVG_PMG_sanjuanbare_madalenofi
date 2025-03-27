#ifndef __CAMERA_COMPONENT__
#define __CAMERA_COMPONENT__

#include "Core/camera.h"
#include "../deps/nlohmann/json.hpp"

struct CameraComponent {

    Camera camera;

    int window_width = 800;
    int window_height = 600;

    // ---

    nlohmann::json serialize();

    CameraComponent();
    ~CameraComponent();
};

#endif // __CAMERA_COMPONENT__
