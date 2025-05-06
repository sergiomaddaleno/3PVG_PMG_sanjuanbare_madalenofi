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
#include "Components/transform_component.h"
#include <../deps/glm/glm.hpp>

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Triangle {

public:

	enum Color {
		Red,
		Green,
		Blue,
		Yellow,
		Other,
	};

	struct PointsTriangle {
		glm::vec2 pos_;
		glm::vec3 rgb;
	};

	PointsTriangle my_triangle[3];

	static std::optional<Triangle> make(float size_, Color color);
	void bind();
	void draw();

	void render(Shader shader, TransformComponent tc, Camera cam = Camera()) {
		shader.useProgram();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, tc.position);
		model *= glm::mat4_cast(tc.rotation);
		model = glm::scale(model, tc.scale);

		glm::mat4 view = cam.getViewMatrix();
		glm::mat4 projection = cam.getPerspectiveMatrix();

		shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
		shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
		shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

		this->draw();

		glUseProgram(0);
	}

	Triangle(PointsTriangle t[3]);
	~Triangle();
	Triangle(const Triangle& other);

private:

	GLuint VAO, VBO;

};

#endif // __TRIANGLE__