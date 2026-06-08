#pragma once
#include "ObjectModel.h"
class ObjectShader;

 
class GameObject :
	public ObjectModel
{
public:
	GameObject();
	GameObject(ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename);
	~GameObject();
	void Update();
	void SynchronizeTransform();
	bool Draw();


private:
	ObjectShader* mpShaderManager;

};

