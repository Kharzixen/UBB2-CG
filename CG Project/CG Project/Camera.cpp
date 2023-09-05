
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
		// n�zeti transzform�ci� be�ll�t�sa
		float t = SDL_GetTicks() / 1000.0f;
		return glm::lookAt(glm::vec3(8 * cos(t), 5, 8 * sin(t)),		// honnan n�zz�k a sz�nteret
			glm::vec3(0, 0, 0),		// a sz�nt�r melyik pontj�t n�zz�k
			glm::vec3(0, 1, 0));		// felfel� mutat� ir�ny a vil�gban
	}
	else {
		return glm::lookAt(glm::vec3(8 * cos(0), 5, 8 * sin(0)),		// honnan n�zz�k a sz�nteret
			glm::vec3(0, 0, 0),		// a sz�nt�r melyik pontj�t n�zz�k
			glm::vec3(0, 1, 0));		// felfel� mutat� ir�ny a vil�gban	// felfel� mutat� ir�ny a vil�gban
	}
}