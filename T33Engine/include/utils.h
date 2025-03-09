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

class Shader;

enum TextureID {
	TI_ShadowMap,
	TI_Albedo,
	TI_Diffuse,
	TI_Normal,
	TI_Specular,
	TI_Position,
	TI_Parallax
};

enum ParallaxType {
	ParallaxMapping,
	SteepParallaxMapping,
	ParallaxOcclusionMapping,
};

enum FrameBufferID {
	FB_Main,
	FB_Quad,
	//...
};

/**
 * @brief Reads the contents of a file and returns its content.
 */
char* ReadFile(const char* file_path = nullptr);

/**
 * @brief Initializes a shader program by reading and compiling.
 */
void initShader(Shader& shader, std::string vertrex_path, std::string fragment_path);

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

#endif // __UTILS__