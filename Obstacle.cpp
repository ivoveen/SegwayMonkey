#include "Obstacle.h"


Obstacle::Obstacle(vec3 position, BulletManager* bulletManager, ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename) : GameObject::GameObject(shaderManager, FileName, shaderFilename) {
	worldPosition = position;

}