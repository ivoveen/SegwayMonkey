#include "TerrainModel.h"
#include"ObjectShader.h"

TerrainModel::TerrainModel(ObjectShader* pShaderManager, std::vector<float>& meshData, std::vector<unsigned int>& meshIndices, std::vector<float>& meshNormals, std::vector<float>& meshColor) {
	mpShaderManager = pShaderManager;

	//vertices
	glGenBuffers(1, &o.vb);
	glBindBuffer(GL_ARRAY_BUFFER, o.vb);
	glBufferData(GL_ARRAY_BUFFER,
		meshData.size() * sizeof(float),
		meshData.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//indeces
	glGenBuffers(1, &IndicesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		meshIndices.size() * sizeof(float),
		meshIndices.data(), 
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Normals
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		meshNormals.size() * sizeof(float),
		meshNormals.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	 
	//Colors
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		meshColor.size() * sizeof(float),
		meshColor.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	amountOfIndeces = meshIndices.size();
	//printf("THIS IS AMOUNT OF INDECES %i", amountOfIndeces/6);
}


TerrainModel::~TerrainModel() {
	glDeleteBuffers(1, &o.vb);
	glDeleteBuffers(1, &IndicesEBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &colorVBO);
}

void TerrainModel::Update() { 
//------------------------------
//update matrices
//------------------------------
	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, scales);
	MakeRotationMatrix();
	MakeTranslationMatrix();
	MakeModelMatrix();
	//printf("Scales: x: %f, y: %f, z:  %f \n", Scales.x, Scales.y, Scales.z);
	//printf("Position: x: %f, y: %f, z:  %f \n", WorldPosition.x, WorldPosition.y, WorldPosition.z);
	//printf("Rotations: x: %f, y: %f, z:  %f \n", Rotations.x, Rotations.y, Rotations.z);

}

void TerrainModel::Draw()
{

	mpShaderManager->setMat4("model", model);


	// Use the program object

	mpShaderManager->use(ObjectShader::Terrain);

	//load the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, o.vb);
	glVertexAttribPointer(mpShaderManager->terrainPositionLoc,
		3,
		// write 3 values
		GL_FLOAT,
		// they are floats
		GL_FALSE,
		// not normals
		3 * sizeof(GLfloat),
		//now many bytes till the next
		0 // where does it start
	);  // use 3 values, but add 5 each time to get to the next
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normal
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(mpShaderManager->terrainNormalLoc,
		3,	 // there are 3 values
		GL_FLOAT,	 //they are floats 
		GL_FALSE,	 // we don't need them normalise
		3 * sizeof(GLfloat),		  // wats the stride to the next ones
		0  //(GLfloat*)this->Vertices + 9 // where is the 1st one);
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Color
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(mpShaderManager->terrainColorLoc,
		3,	 // there are 3 values
		GL_FLOAT,	 //they are floats 
		GL_FALSE,	 // we don't need them normalise
		3 * sizeof(GLfloat),// whats the stride to the next ones
		0  //(GLfloat*)this->Vertices + 9 // where is the 1st one);
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glEnableVertexAttribArray(mpShaderManager->terrainPositionLoc);
	glEnableVertexAttribArray(mpShaderManager->terrainNormalLoc);
	glEnableVertexAttribArray(mpShaderManager->terrainColorLoc);

	//glEnableVertexAttribArray(this->texCoordLoc);

	//glBindTexture(GL_TEXTURE_2D, this->texture1);
	glUniform1i(mpShaderManager->terrainSamplerLoc, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesEBO);
	glDrawElements(GL_TRIANGLES, amountOfIndeces, GL_UNSIGNED_INT, 0);
	GLenum err = glGetError();
	if  (err != GL_NO_ERROR)
	{
		printf("Oh fiddlesticks terrain Draw error, %i\n", err);
	}
}