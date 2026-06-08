#pragma once
#include "GameObject.h"

class btKinematicCharacterController;
class btPairCachingGhostObject;
class Player :
    public GameObject
{
public:
    Player(vec3 Pos, ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename);

    Player(vec3 Pos, BulletManager* mBulletManager, ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename);
    ~Player();
    void UpdateMovement(float deltaTime, std::vector<int>* keys);
    void UpdateShowcaseMovement(float deltaTime);
    float groundFriction = 3.0f;
    bool grounded = true;
    bool goingDown = false;
private:
    BulletManager* mBulletManager;
};

