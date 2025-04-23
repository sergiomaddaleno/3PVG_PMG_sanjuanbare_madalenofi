#ifndef __LIGHT_COMPONENT__
#define __LIGHT_COMPONENT__

#include "../Render/light.h"
#include "Render/shadow_map.h"

#include "../deps/nlohmann/json.hpp"

struct LightComponent {
    Light light;
    std::shared_ptr<ShadowMap> shadow_map_;
    nlohmann::json serialize();
};

#endif //__LIGHT_COMPONENT__