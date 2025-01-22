
#ifndef PRT
#define PRT

#include <optional>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Render/camera.h"

#include <../deps/glm/glm.hpp>

struct RenderComponent;
struct TransformComponent;

class Triangle {

public:
	 
	struct PointsTriangle {
		glm::vec2 pos_;
		glm::vec3 rgb;
	};

	enum Color {
		Red,
		Green,
		Blue,
		Yellow,
		Other,
	};

	PointsTriangle my_triangle[3];   

	static std::optional<Triangle> make(float size_, Color color);

	void render(RenderComponent rc, TransformComponent tc, Camera cam = Camera());

	// ---

	Triangle(PointsTriangle t[3]) {
		for (int i = 0; i < 3; ++i) {
			my_triangle[i] = t[i];
		}
	}

	Triangle(const Triangle& other) {
        for (int i = 0; i < 3; ++i) {
            my_triangle[i] = other.my_triangle[i];
        }
    }

	~Triangle() {};
};

#endif // PRT