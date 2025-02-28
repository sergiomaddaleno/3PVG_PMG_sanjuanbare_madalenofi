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
	TI_Normal,
	TI_Specular,
	//...
};

enum FrameBufferID {
	FB_Main,
	FB_Quad,
	//...
};

char* ReadFile(const char* file_path = nullptr);

void initShader(Shader& shader, std::string vertrex_path, std::string fragment_path);

// usage example : CheckGPUErrors(__FILE__, __LINE__);
void CheckGPUErrors(const char* file, int line);

void CheckShaderErrors(GLuint shader, const char* shaderType);

void CheckProgramErrors(GLuint program);

#endif // __UTILS__