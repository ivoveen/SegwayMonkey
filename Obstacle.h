#pragma once
#include "GameObject.h"
class BulletManager;
class ObjectShader;

class Obstacle :
    public GameObject
{
public:

    Obstacle(vec3 Pos, BulletManager* mBulletManager, ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename);
    bool active = false;
    int mChunkID = 0;
    int mZoneID = 0;

private:

};

