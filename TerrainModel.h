#pragma once
#include "Model.h"
class ObjectShader;

class TerrainModel : public Model
{
public:
	TerrainModel(ObjectShader* pShaderManager, std::vector<float>& meshData, std::vector<unsigned int>& meshIndices, std::vector<float>& meshNormals, std::vector<float>& meshColor);
	~TerrainModel();
	void Draw();
	void Update();
	 
private:
	ObjectShader* mpShaderManager;
	DrawObject o;
	unsigned int IndicesEBO;
	unsigned int normalVBO;
	unsigned int colorVBO;

	int amountOfIndeces= 0;
};

