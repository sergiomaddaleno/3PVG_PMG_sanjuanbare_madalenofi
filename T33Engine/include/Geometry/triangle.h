/**
 * @file triangle.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __TRIANGLE__
#define __TRIANGLE__

#include <optional>
#include <string>

#include "Core/camera.h"
#include "Core/shader.h"
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct RenderComponent;
struct TransformComponent;

class Triangle {

public:

	struct PointsTriangle {
		glm::vec2 pos_;
		glm::vec3 rgb;
	};
	 
	Triangle(PointsTriangle t[3]);
	~Triangle();
	Triangle(const Triangle& other);

	enum Color {
		Red,
		Green,
		Blue,
		Yellow,
		Other,
	};

	static std::optional<Triangle> make(float size_, Color color);

	void render(Shader shader, RenderComponent rc, TransformComponent tc, Camera cam = Camera());

	PointsTriangle my_triangle[3];   
};

#endif // __TRIANGLE__