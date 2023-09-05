
#include "Camera.h"


MyCamera::MyCamera()
{
}

MyCamera::~MyCamera()
{
}


void MyCamera::setAutoOn() {
	autoMovement = true;
}

void MyCamera::setAutoOff() {
	autoMovement = false;
}


bool MyCamera::isAutoOn() {
	return this -> autoMovement;
}


glm::highp_mat4 MyCamera::Update() {
	if (this->autoMovement) {
		// nézeti transzformáció beállítása
		float t = SDL_GetTicks() / 1000.0f;
		return glm::lookAt(glm::vec3(8 * cos(t), 5, 8 * sin(t)),		// honnan nézzük a színteret
			glm::vec3(0, 0, 0),		// a színtér melyik pontját nézzük
			glm::vec3(0, 1, 0));		// felfelé mutató irány a világban
	}
	else {
		return glm::lookAt(glm::vec3(8 * cos(0), 5, 8 * sin(0)),		// honnan nézzük a színteret
			glm::vec3(0, 0, 0),		// a színtér melyik pontját nézzük
			glm::vec3(0, 1, 0));		// felfelé mutató irány a világban	// felfelé mutató irány a világban
	}
}