#ifndef LCP
#define LCP

#include <memory>
#include "../Render/shader.h"
#include "../Render/light.h"

struct LightComponent {

	LightInfo light_info;
	Shader shader;

};

#endif //LCP