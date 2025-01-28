#ifndef LCP
#define LCP

#include <memory>
#include "../Core/shader.h"
#include "../Render/light.h"

#include "../deps/nlohmann/json.hpp"

struct LightComponent {

	LightInfo light_info;
	Shader shader;

	nlohmann::json serialize() {

		nlohmann::json json_data;

		json_data["light_info"] = light_info.serialize();
		json_data["shader"] = shader.serialize();

		return json_data;
	}

};

#endif //LCP