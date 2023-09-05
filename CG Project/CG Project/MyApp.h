#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "Camera.h"

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:

	// shaderekhez szükséges változók
	GLuint m_programID = 0; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID = 0; // vertex array object erőforrás azonosító
	GLuint m_vboID = 0; // vertex buffer object erőforrás azonosító
	GLuint m_ibID = 0;  // index buffer object erőforrás azonosító
	// OpenGL-es dolgok
	GLuint m_vaoID1 = 0; // vertex array object erőforrás azonosító
	GLuint m_vboID1 = 0; // vertex buffer object erőforrás azonosító
	GLuint m_ibID1 = 0;  // index buffer object erőforrás azonosító

	// transzformációs mátrixok
	glm::mat4 m_matWorld = glm::mat4(1.0f);
	glm::mat4 m_matView = glm::mat4(1.0f);
	glm::mat4 m_matProj = glm::mat4(1.0f);

	// mátrixok helye a shaderekben
	GLuint	m_loc_mvp = 0; // a három mátrixunk szorzatát adjuk át a hatékonyság érdekében

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};

	//current window size;

	int width; 
	int height;

	//render mode;

	int renderMode = 0;

	glm::vec4 balls[5];
	
	//metalabdák tömb helye a shaderben
	//threshold helye a shaderben
	GLuint m_metaballs = 0; 
	GLuint m_tr = 0;
	GLuint m_res = 0;
	GLuint m_renderMode = 0;
	GLuint m_lightSources = 0;

	GLuint loc_eye;
	GLuint loc_at;
	GLuint loc_up;

	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 at;
};


