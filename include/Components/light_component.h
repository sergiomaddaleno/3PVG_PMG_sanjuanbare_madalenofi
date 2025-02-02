#ifndef LCP
#define LCP

#include <memory>
#include "../Core/shader.h"
#include "../Render/light.h"
#include "../Core/framebuffer.h"

#include "../deps/nlohmann/json.hpp"

struct LightComponent {

	Light light;
	Shader shader;
	FrameBuffer shadow_map;

	// To save data
	nlohmann::json serialize() {

		nlohmann::json json_data;

		json_data["light"] = light.serialize();
		json_data["Shader"] = shader.serialize();

		return json_data;
	}

};

#endif //LCP