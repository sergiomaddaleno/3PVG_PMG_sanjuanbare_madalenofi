/**
 * @file utils.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __UTILS__
#define __UTILS__

#include <memory> 
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "../deps/nlohmann/json.hpp"
#include <../deps/glm/glm.hpp>

class Shader;

enum TextureID {
	TI_Diffuse,
	TI_Position,
	TI_Normal,
	TI_Albedo,
	TI_ShadowMap,
	TI_Parallax,
	TI_ShadowMapCube,
	TI_Screen_Color,
	TI_Screen_Depth,
	TI_Noise,
	TI_SSAO
};

enum WhatToRender {
	WTR_forward,
	WTR_deferred,
	WTR_ssao,
};

enum ParallaxType {
	ParallaxMapping,
	SteepParallaxMapping,
	ParallaxOcclusionMapping,
};

struct EntityCounters {
	int camera = 0;
	int skybox = 0;
	int cloud = 0;
	int mesh = 0;
	int terrain = 0;
	int water = 0;
	int sphere = 0;
	int surface = 0;
	int cube8v = 0;
	int cube24v = 0;
	int quad = 0;
	int spotlight = 0;
	int pointlight = 0;
	int dirlight = 0;
};

/**
 * @brief Reads the contents of a file and returns its content.
 */
char* ReadFile(const char* file_path = nullptr);

/**
 * @brief Initializes a shader program by reading and compiling.
 */
void initShader(Shader& shader, const std::string vertrex_path, const std::string fragment_path, const std::string geometry_path = "");

void AddConsoleMessage(const std::string& message);
const std::vector<std::string>& GetConsoleMessages();

/**
 * @brief Checks for OpenGL errors and prints them if any.
 * @param file The name of the file where the check is being made.
 * @param line The line number where the check is being made.
 */
void CheckGPUErrors(const char* file, int line);

/**
 * @brief Checks for shader compilation errors and prints them if any.
 * @param shaderType A string describing the type of shader ("Vertex", "Fragment" ...).
 */
void CheckShaderErrors(GLuint shader, const char* shaderType);

/**
 * @brief Checks for shader program linking errors and prints them if any.
 */
void CheckProgramErrors(GLuint program);

glm::vec3 parseVec3(const nlohmann::json& j);

glm::quat parseQuat(const nlohmann::json& j);

#endif // __UTILS__