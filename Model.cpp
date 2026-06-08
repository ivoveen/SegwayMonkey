#include "Model.h"

Model::Model() {
	scales = glm::vec3(1.0f, 1.0f, 1.0f);
	worldPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	rotations = glm::vec3(0.0f, 0.0f, 0.0f);
	model = glm::mat4(1.0f);
	SetModelMatrix();
}

void Model::CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
	float v10[3];
	v10[0] = v1[0] - v0[0];
	v10[1] = v1[1] - v0[1];
	v10[2] = v1[2] - v0[2];

	float v20[3];
	v20[0] = v2[0] - v0[0];
	v20[1] = v2[1] - v0[1];
	v20[2] = v2[2] - v0[2];

	N[0] = v20[1] * v10[2] - v20[2] * v10[1];
	N[1] = v20[2] * v10[0] - v20[0] * v10[2];
	N[2] = v20[0] * v10[1] - v20[1] * v10[0];

	float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
	if (len2 > 0.0f) {
		float len = sqrtf(len2);

		N[0] /= len;
		N[1] /= len;
	}
}


// create the model by load the rotations/positions/scale matrices
void	Model::SetModelMatrix()
{
	//set the matrices we use to I	
	mTranslationMatrix = glm::mat4(1.0f);
	mRotationMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::mat4(1.0f);

	// set scale matrix
	mScaleMatrix = glm::scale(mScaleMatrix, scales);  // nice easy function
	// set Rotation Matrix	
	MakeRotationMatrix();  // this is a bit complex so go do them
	// set translation
	mTranslationMatrix = glm::translate(mTranslationMatrix, worldPosition);  //use the glm translate

	// now make the Model for use in render
	model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;

}

void Model::MakeTranslationMatrix()
{
	// this won't work
	mTranslationMatrix = glm::mat4(1.0f);
	mTranslationMatrix[3][0] = worldPosition.x;
	mTranslationMatrix[3][1] = worldPosition.y;
	mTranslationMatrix[3][2] = worldPosition.z;
}

// do the rotation matrices
void Model::MakeRotationMatrix()
{
	rotations.x += offsetXRotation;
	// set rotationx
	RotationMatrixX = glm::mat4(1.0f);
	RotationMatrixX[1][1] = cosf(rotations.x);
	RotationMatrixX[2][1] = -sinf(rotations.x);
	RotationMatrixX[1][2] = sinf(rotations.x);
	RotationMatrixX[2][2] = cosf(rotations.x);
	//set rotationy
	RotationMatrixY = glm::mat4(1.0f);
	RotationMatrixY[0][0] = cosf(rotations.y);
	RotationMatrixY[2][0] = sinf(rotations.y);
	RotationMatrixY[0][2] = -sinf(rotations.y);
	RotationMatrixY[2][2] = cosf(rotations.y);
	//set rotationz
	RotationMatrixZ = glm::mat4(1.0f);
	RotationMatrixZ[0][0] = cosf(rotations.z);
	RotationMatrixZ[1][0] = -sinf(rotations.z);
	RotationMatrixZ[0][1] = sinf(rotations.z);
	RotationMatrixZ[1][1] = cosf(rotations.z);

	// now the combined rotation	
	mRotationMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;
}


// simply create the model matrix, assumes the translations and others have been set
void Model::MakeModelMatrix()
{
	model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
}


// return the position info from worldposition
glm::vec3	Model::GetPosition()
{
	return worldPosition;
}

//set the word position
void		Model::SetPosition(glm::vec3* a_Pos)
{
	worldPosition.x = a_Pos->x;
	worldPosition.y = a_Pos->y;
	worldPosition.z = a_Pos->z;
}
//set the world position
void		Model::SetPosition(glm::vec3 a_Pos)
{
	worldPosition = a_Pos;
}

glm::vec3	Model::GetRotations()
{
	return rotations;
}
void		Model::SetRotations(glm::vec3* a_Rots)
{
	rotations.x = a_Rots->x;
	rotations.y = a_Rots->y;
	rotations.z = a_Rots->z;
}

void		Model::SetRotations(glm::vec3 a_Rots)
{
	rotations = a_Rots;
}

void		Model::SetXRotation(float a_x)
{
	offsetXRotation = a_x;
}
void		Model::SetYRotation(float a_y)
{
	rotations.y = a_y;
}
void		Model::SetZRotation(float a_z)
{
	rotations.z = a_z;
}
