#pragma once
#include<vector>
#include <sys/time.h>

class ObjectShader;
class Player;
class Camera;
class Target_State;
class Graphics;
class Obstacle;

class GameStatePause
{

public:
	GameStatePause(Graphics* graphics, ObjectShader* shaderManager, Target_State* pState);
	int Update(float deltaTime);

private:
	float fogDistance = 780;
	float radius = 3000;
	float ambientPercent = 1;

	Graphics* mGraphics;
	Target_State* p_state;
	ObjectShader* mpShaderManager;
	Player* mPlayer;
	Camera* mpCamera;
	float totalTime = 0;
	float fps = 0;
	unsigned int frames = 0;
	std::vector<int>* keys = 0;
};




