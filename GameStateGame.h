#pragma once
#include<vector>
#include <sys/time.h>

class ObjectShader;
class BulletManager;
class WorldManager;
class Player;
class Camera;
class Target_State;
class Graphics;
class Obstacle;

class GameStateGame
{ 
public:
	GameStateGame() {};
	void Init(Graphics* graphics, ObjectShader* shaderManager, Target_State* pState, int difficulty);
	int update(float deltaTime, std::vector<int>* keys);
	~GameStateGame();

private:
	Graphics* mGraphics;
	Target_State* p_state;
	ObjectShader* mpShaderManager;
	BulletManager* mpBulletManager;
	WorldManager* mWorldManager;
	Player* mPlayer;
	Camera* mpCamera;
	Obstacle* mObstacle;
	float totalTime = 0;
	float fps = 0;
	unsigned int frames = 0;
	bool simulation_running = true;
	bool drawDebugLines = false;
	std::vector<int>* keys = 0;
	float fogDistance = 780;
	float radius = 3000;
	float ambientPercent = 1;
	int i = 0;
};

