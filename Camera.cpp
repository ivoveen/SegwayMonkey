#include "Camera.h"
#include "Graphics.h"


using namespace glm;

Camera::Camera() {
	worldPosition = glm::vec3(77.5f, 5, -11);
	targetPosition = glm::vec3(77.5f, -2.0f, 10);
	rotations = vec3(0, 90, 0);
}

void Camera::Update(std::vector<int>* keys) {
	//if (keys != nullptr) 
	//{

	//	glm::vec3 direction = (targetPosition - worldPosition);
	//	normalize(direction);
	//	direction /= 10;

	//	vec3 sideDirection = vec3(direction.z, direction.y, -direction.x);
	//	vec3 upDirection = vec3(0, 1, 0);

	//	direction *= 0.1;
	//	sideDirection *= 0.1;
	//	upDirection *= 0.1;

	//	for (int i = 0; i < static_cast<int>(keys->size()); i++) {

	//		if (keys->at(i) == XK_q) rotations.y -= 1;
	//		if (keys->at(i) == XK_e) rotations.y += 1;

	//		if (keys->at(i) == XK_z) worldPosition += upDirection, targetPosition += upDirection;
	//		if (keys->at(i) == XK_x) worldPosition -= upDirection, targetPosition -= upDirection;

	//		if (keys->at(i) == XK_w) worldPosition += direction, targetPosition += direction;
	//		if (keys->at(i) == XK_a) worldPosition += sideDirection, targetPosition += sideDirection;
	//		if (keys->at(i) == XK_s) worldPosition -= direction, targetPosition -= direction;
	//		if (keys->at(i) == XK_d) worldPosition -= sideDirection, targetPosition -= sideDirection;

	//	}	//rotationDegree++;
	//	RotateAroundTarget(rotations.y);
	//}

	MakeVPMatrices();
}


void Camera::RotateAroundTarget(float degrees) {
	float rad = DEG2RAD(degrees);
	targetPosition.z = (sin(rad) * CIRCLERADIUS) + worldPosition.z;
	targetPosition.x = (cos(rad) * CIRCLERADIUS) + worldPosition.x;

}



void Camera::MakeVPMatrices() {
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : near 0.1 unit <->  far 100 units
	projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 600.0f);

	view = glm::lookAt(
		worldPosition,
		// Camera is at (17,23,3), in World Space
		targetPosition,
		// look at the origin
		glm::vec3(0, 1, 0)  // pointing up( 0,-1,0 will be upside-down)););}
	);
}

glm::mat4 Camera::MakeMVPMatrix(glm::mat4 model) {
	return projection * view * model;
}

glm::mat4 Camera::MakeVPMatrix() {
	return projection * view;
}
