#ifndef LCP
#define LCP

#include <memory>
#include "shader.h"
#include "light.h"


struct LightComponent {

	Light light_;
	Shader shader;
};

#endif //LCP