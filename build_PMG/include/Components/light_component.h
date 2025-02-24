#ifndef __LIGHT_COMPONENT__
#define __LIGHT_COMPONENT__

#include "../Render/light.h"
#include "Render/shadow_map.h"

#include "../deps/nlohmann/json.hpp"

struct LightComponent {

	Light light;
	ShadowMap shadow_map_;

	// To save data
	nlohmann::json serialize() {
		nlohmann::json json_data;
		json_data["light"] = light.serialize();
		return json_data;
	}
};

#endif //__LIGHT_COMPONENT__