#ifndef LCP
#define LCP

#include <memory>
#include "shader.h"
#include "light.h"


struct LightComponent {

	LightInfo light_info;
	Shader shader;

};

#endif //LCP