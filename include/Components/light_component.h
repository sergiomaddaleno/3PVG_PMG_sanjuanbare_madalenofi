#ifndef LCP
#define LCP

#include <memory>
#include "../Core/shader.h"
#include "../Render/light.h"
#include "Render/shadow_map.h"

#include "../deps/nlohmann/json.hpp"

struct LightComponent {

	Light light;
	Shader shader;
	ShadowMap shadow_map_;

	// To save data
	nlohmann::json serialize() {

		nlohmann::json json_data;

		json_data["light"] = light.serialize();
		json_data["Shader"] = shader.serialize();

		return json_data;
	}
};

#endif //LCP