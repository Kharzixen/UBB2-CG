#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>
#include <vector>


CMyApp::CMyApp()
{
}


CMyApp::~CMyApp()
{
}



bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok


	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés előtt kell ezt megtenni!
	glBindAttribLocation(m_programID,	// shader azonosítója, amiből egy változóhoz szeretnénk hozzárendelést csinálni
		0,				// a VAO-beli azonosító index
		"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation(m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenő-bemenő változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result || infoLogLength != 0)
	{
		std::vector<char> VertexShaderErrorMessage(infoLogLength);
		glGetProgramInfoLog(m_programID, infoLogLength, nullptr, VertexShaderErrorMessage.data());
		std::cerr << "[glLinkProgram] Shader linking error:\n" << &VertexShaderErrorMessage[0] << std::endl;
	}
	// mar nincs ezekre szukseg
	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);

	//
	// egyéb inicializálás
	//

	// shader-beli transzformációs mátrixok címének lekérdezése
	m_loc_mvp = glGetUniformLocation(m_programID, "MVP");

	//shader beli metaball dolgok
	m_tr = glGetUniformLocation(m_programID, "tr");
	m_metaballs = glGetUniformLocation(m_programID, "balls");
	m_res = glGetUniformLocation(m_programID, "resolution");
	m_renderMode = glGetUniformLocation(m_programID, "mode");
	m_lightSources = glGetUniformLocation(m_programID, "lightSources");

	loc_eye = glGetUniformLocation(m_programID, "eye");
	loc_at = glGetUniformLocation(m_programID, "at");
	loc_up  = glGetUniformLocation(m_programID, "up");

	eye = glm::vec3(0, 0.3, 5);
	at  = glm::vec3(0, 0, 0);
	up  = glm::vec3(0, 1, 0);

	return true;
}

void CMyApp::Clean()
{
	glDeleteProgram(m_programID);
}

void CMyApp::Update()
{
	m_matView = glm::lookAt(glm::vec3(0, 0, 0.9),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));
	
	for (int i = 0; i < 5; i++) {
		balls[i] = glm::vec4(sin(SDL_GetTicks()/2000.0 * .5 + .967 + 42. * (float(i))),
			cos(SDL_GetTicks() / 2000.0 * .5 * .423 + 152. * (float(i))),
			-10 + cos(.76321 * SDL_GetTicks() / 3000.0 * .5 + (float(i))),
			1.3);//(sin(SDL_GetTicks() / 4000.0)*0.5 + 1.9 )/2.0);
	}

}


void CMyApp::Render()
{

	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa, ebben a projektben a teljes programot jelöli, hisz nem váltunk a shaderek között
	glUseProgram(m_programID);

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/
	m_matWorld = glm::mat4(1.0f);

	glm::mat4 mvp =m_matProj * m_matView;

	// majd küldjük át a megfelelő mátrixot!
	// Uniform változó bindolása csak a program bindolása után lehetséges! (glUseProgram() )
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
	
	
	glUniform4fv(m_metaballs, 5,&balls[0].x);

	glUniform2f(m_res, float(width), float(height));

	glUniform1f(m_tr, SDL_GetTicks() / 1000.f);

	glUniform1i(m_renderMode, renderMode);

	glUniform3f(loc_eye, eye[0], eye[1], eye[2]);
	glUniform3f(loc_at, at[0], at[1], at[2]);
	glUniform3f(loc_up, up[0], up[1], up[2]);

	glm::vec3 lightSources[3] = {
		glm::vec3(2,6,10),
		glm::vec3(-22,0,2),
		glm::vec3(0,-6,-3),
	};


	glUniform3fv(m_lightSources, 3, &lightSources[0].x);

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	//glBindVertexArray(m_vaoID);

	// kirajzolás
	//A draw hívásokhoz a VAO és a program bindolva kell legyenek (glUseProgram() és glBindVertexArray())


	glDrawArrays(GL_TRIANGLES,	// primitív típusa; amikkel mi foglalkozunk: GL_POINTS, GL_LINE_STRIP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES
		0,					// ha van tároló amiben a kirajzolandó geometriák csúcspontjait tároljuk, akkor annak hányadik csúcspontjától rajzoljunk - most nincs ilyen, 
		// csak arra használjuk, hogy a gl_VertexID számláló a shader-ben melyik számról induljon, azaz most nulláról
		6);

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{	
	int MAX_MODE = 4;
	glm::vec3 forward = at - eye;
	glm::vec3 side = glm::cross(forward, up);

	switch (key.keysym.sym)
	{
	case(SDLK_LEFT):
		if (renderMode > 0) {
			renderMode--;
		}
		else {
			renderMode = MAX_MODE;
		}
		break;
	case(SDLK_RIGHT):
		if (renderMode < MAX_MODE) {
			renderMode++;
		}
		else {
			renderMode = 0;
		}
		break;
	case(SDLK_ESCAPE):
		break;
	case(SDLK_s):
		eye += forward * 0.1f;
		break;
	case(SDLK_w):
		eye -= forward * 0.1f;
		break;
	case(SDLK_a):
		eye += side * 0.1f;
		break;
	case(SDLK_d):
		eye -= side * 0.1f;
		break;
	default: break;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	width = _w; 
	height = _h;

	glViewport(0, 0, _w, _h);

	// vetítési mátrix beállítása
	m_matProj = glm::perspective(glm::radians(60.0f),	// 60 fokos nyílásszög radiánban
		_w / (float)_h,			// ablakméreteknek megfelelő nézeti arány
		0.01f,					// közeli vágósík
		1000.0f);				// távoli vágósík
}