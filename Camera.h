#pragma once
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof
#include<vector>

using namespace glm;

class Camera
{
public:
	Camera();
	void Update(std::vector<int>* keys);
	void RotateAroundTarget(float degrees);
	void MakeVPMatrices();
	glm::mat4 MakeMVPMatrix(glm::mat4 model);
	glm::mat4 MakeVPMatrix(); 


	glm::vec3	worldPosition;   // where am I in the world?

private:
	glm::vec3	targetPosition;   // where am I in the world?
	glm::vec3	rotations; 		// what are my rotations? Stored as Radians!
	glm::vec3	scales;

	glm::mat4 projection;
	glm::mat4 view;

	//circle movement variables
	const float CIRCLERADIUS = 50;
	int rotationDegree = 0;

};

