#pragma once

#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f
#define DEG2RAD(x) (x*PI)/180
#define RAD2DEG(x) x*(180/PI)
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof
#include <GLES2/gl2.h>
#include <vector>


using namespace glm;



class Model
{
public:

	typedef struct {
		// this struct defines our drawobjects as having a VB(o), a number of triangles, and a matriel ID, which is an index to a vector that will
		// grow as we load more and more objects with new materials

		GLuint vb;     // vertex buffer
		int numTriangles;
		size_t material_id;
	} DrawObject;

	
	glm::vec4	Colour;
	glm::vec3	worldPosition;   // where am I in the world?
	float offsetY = 0;
	glm::vec3	rotations; 		// what are my rotations? Stored as Radians!
	float offsetXRotation = 0;
	glm::vec3	scales;

	glm::mat4 mTranslationMatrix;
	glm::mat4 mRotationMatrix;
	glm::mat4 mScaleMatrix;

	glm::mat4 RotationMatrixX;
	glm::mat4 RotationMatrixY;
	glm::mat4 RotationMatrixZ;


	glm::mat4 model;      // the model matrix will effectivly hold all the rotation and positional data for the object

	// Ambiant light is universal in all model types, so we should keep this in the base class	
	float Amb[4];  // = { Rand(1.0f), Rand(1.0f), Rand(1.0f), 1.0f };



	Model();
	void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]);


	/******************** Back to normal standard objects content*/
// a few basic utility functions	
	glm::vec3	GetPosition();
	void		SetPosition(glm::vec3*);
	void		SetPosition(glm::vec3);

	glm::vec3	GetRotations();
	void		SetRotations(glm::vec3*);
	void		SetRotations(glm::vec3);

	void		SetXRotation(float);  // if you need to set indivual rotations
	void		SetYRotation(float);
	void		SetZRotation(float);

	void		SetModelMatrix();  // initialises and resets
	void	 	MakeModelMatrix();  // makes and returns the model matrix
	void		MakeRotations();
	void		MakeRotationMatrix();  // since these get altered alot

	void		MakeTranslationMatrix();
};

