#pragma once
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Obstacle.h"
#undef countof
#include <vector>
class Zone;
class ObjectShader;
class GameObject;
class BulletManager;
class Chunk;
#include "ContactCallback.h"
class Player;

class WorldManager
{
public:
	WorldManager(ObjectShader* shaderManager, BulletManager* mpBulletManager, int difficutlty, Player* player);
	~WorldManager();

	std::vector<Zone*> mWorldZones;
	void GenerateWorld(int difficulty);
	void CreateHitboxes(glm::vec3 Pos, glm::vec3 scale);
	void Update(float deltaTime);
	void Draw();
	void GenerateObjects();
	void DrawObjects();

	void SyncDifficulty();

	float mScore = 0;

	int mDifficulty = 1;
	float difficultyFactor = 0;
	float mSpeed = 0.2f;
	int mSelectedZoneID = 0, mSelectedChunkID = 0, mCurrentZoneID = 0;
	std::vector<Chunk*> chunksInTheWorld;
	ContactCallback collisionCallback;

private:
	uint seed = 0;
	uint objectSeed = 0;
	std::vector<GameObject*> groundHitboxes;
	ObjectShader* mpShaderManager;
	BulletManager* mpBulletManager;
	int amountOfZones = 10;
	const int AMOUNTOFCHUNKSINTHEWORLD = 3;
	Player* mPlayer;
	std::vector<Obstacle*> mObstacles;
	std::vector<Obstacle*> mActiveObstacles;




};

