/**
 * @file post_processing.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __POST_PROCESS__
#define __POST_PROCESS__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/shader.h"
#include "Core/framebuffer.h"

#include <string>
#include <iostream>
#include <vector>
#include <../deps/glm/glm.hpp>

#include "Geometry/quad.h"

 /**
  * @class PostProcessing
  * @brief Implements post-processing effects for rendered scenes.
  */
class PostProcessing {
public:

	PostProcessing();
	~PostProcessing();

	void initData();

	void beginRender();
	void endRender();

	void render(Shader& shader);
	void resize(unsigned int width, unsigned int height);

	float grey_;
	float negative_;
	float waves_;
	float blur_;
	int kernel_types_;

	FrameBuffer frame_buffer_;

private:
	unsigned int width_, height_;

	Quad screen_quad_;

	GLuint RBO;
};

#endif // __POST_PROCESS__
