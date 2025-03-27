#ifndef __SKYBOX_COMPONENT__
#define __SKYBOX_COMPONENT__

#include "Render/skybox.h"
#include "../deps/nlohmann/json.hpp"

struct SkyboxComponent {

    Skybox skybox;

    // ---

    nlohmann::json serialize();
    void deserialize(const nlohmann::json& cam);

    SkyboxComponent();
    ~SkyboxComponent();
};

#endif // __SKYBOX_COMPONENT__
