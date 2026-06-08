#include "GameObject.h"
#include "PhysicsObj.h"
#include "ObjectShader.h"

GameObject::GameObject() {
	// a default system
	
	scales = glm::vec3(1.0f, 1.0f, 1.0f);
	worldPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	model = glm::mat4(1.0f);  // give the model I as an initialisation
	

	scales = glm::vec3(1.0f, 1.0f, 1.0f);
	SetModelMatrix();

}

GameObject::GameObject(ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename)
{ // because we decided to have an OBJ LoadandConvert method in the base ObjectModel class, we can use that base class method to do our work for us
 // keeping this derived class simple
	mpShaderManager = shaderManager;
	bool ret = ObjectModel::LoadandConvert(&gdrawObjects, materials, textures, FileName);

	if (ret == false)
	{
		printf("This load for %s failed, check file name and try again!\n", FileName);

	}
}


GameObject::~GameObject() {

}

void GameObject::SynchronizeTransform()
{
	//----------------------------------------------------------------------------
	// This function was written by Brian Beuken and is not my own code
	//----------------------------------------------------------------------------

	btScalar transform[16];  // physics uses a 16 float matrix system
	this->MyPhysObj->GetTransform(transform);

	transform[13] -= offsetY;

	this->worldPosition.x = transform[12];
	this->worldPosition.y = transform[13] ;
	this->worldPosition.z = transform[14];  // don't need the ww.

	btQuaternion orient = this->MyPhysObj->GetRigidBody()->getOrientation();
	btVector3 rotation;
	orient.getEulerZYX(rotation[2], rotation[1], rotation[0]);
	SetRotations(vec3(rotation.getX(), rotation.getY(), rotation.getZ()));


	for (int i = 0; i < 16; i++) // we now transfer our physics matrix 
		model[i / 4][i % 4] = transform[i];

	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, scales);  // nice easy function
	glm::mat4 T = model * mScaleMatrix;
	model = T;
}

void GameObject::Update()
{
	if (MyPhysObj != NULL) {
		SynchronizeTransform();
	}
	//------------------------------
	//update matrices
	//------------------------------
	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, scales);
	MakeRotationMatrix();
	MakeTranslationMatrix();
	MakeModelMatrix();
}


bool GameObject::Draw()
{

	mpShaderManager->setMat4("model", model);


	for (int frames = 0; frames < static_cast<int>(gdrawObjects.size()); frames++)

	{
		//printf("draw size: %i", gdrawObjects.size()); //there is 1 gdrawObject
		DrawObject o = gdrawObjects[frames];  // get the DrawObject which has the vb
		glBindBuffer(GL_ARRAY_BUFFER, o.vb);  // this is now the buffer we work with

		GLsizei stride = (3 + 3 + 3 + 2) * sizeof(GLfloat);  // we have 3x vert, 3x normal,3xcolour,2x texcoord.
		//load the vertex data info
		glVertexAttribPointer(mpShaderManager->positionLoc,
			3,		// there are 3 values xyz
			GL_FLOAT, // they a float
			GL_FALSE,	 // don't need to be normalised
			stride,	  // how may floats to the next one
			(const void*)0  //this->Vertices // where do they start as an index); // use 3 values, but add 5 each time to get to the next
		);

		// Load the normals
		glVertexAttribPointer(mpShaderManager->normalLoc,
			2,	 // there are 2 values
			GL_FLOAT,	 //they are floats 
			GL_FALSE,	 // we don't need them normalise
			stride,		  // wats the stride to the next ones
			(const void*)(sizeof(float) * 3)  //(GLfloat*)this->Vertices + 9 // where is the 1st one);
		); 

		// Load the texture coordinate
		glVertexAttribPointer(mpShaderManager->texCoordLoc,
			2,	 // there are 2 values
			GL_FLOAT,	 //they are floats 
			GL_FALSE,	 // we don't need them normalise
			stride,		  // wats the stride to the next ones
			(const void*)(sizeof(float) * 9)  //(GLfloat*)this->Vertices + 9 // where is the 1st one);
		);

		glEnableVertexAttribArray(mpShaderManager->positionLoc);
		glEnableVertexAttribArray(mpShaderManager->texCoordLoc);
		glEnableVertexAttribArray(mpShaderManager->normalLoc);


		// bind the correct texture for this shape	
		if ((o.material_id < materials.size())) {
			std::string diffuse_texname = materials[o.material_id].diffuse_texname;
			if (textures.find(diffuse_texname) != textures.end()) {
				glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
			}
		}
		glUniform1i(mpShaderManager->samplerLoc, 0);
		glDrawArrays(GL_TRIANGLES, 0, o.numTriangles);
		if (glGetError() != GL_NO_ERROR)
		{
			printf("Oh bugger Ship Model Draw error\n");
			return false;
		}

		// take these off the bind		
		glDisableVertexAttribArray(mpShaderManager->positionLoc);
		glDisableVertexAttribArray(mpShaderManager->texCoordLoc);
		glDisableVertexAttribArray(mpShaderManager->normalLoc);


		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/*if (glGetError() != GL_NO_ERROR)
		{
			printf("Oh bugger Ship Model Draw error\n");
			return false;
		}*/

	}
	// always good to unbind	
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}


