#pragma once

#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

class MyCamera
{
public:
	MyCamera();
	~MyCamera();

	bool Init();
	void Clean();

	void setAutoOn(); 
	void setAutoOff(); 

	bool isAutoOn();
	glm::highp_mat4 Update();
protected: 
	bool autoMovement = false;
};