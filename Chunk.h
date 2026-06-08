#pragma once
#include "ZoneType.h"
#include <vector>
#include <string>
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof
#include "TerrainModel.h"
class ObjectShader;
class Obstacle;
class BulletManager;

class Chunk
{
public:

	struct SpawnPoint
	{
		glm::vec3 chunkPosition;
		bool enabled;
	};


	Chunk(ObjectShader* shaderManager, BulletManager* bulletManager, ZoneType typeOfZone, int zoneID, int chunkID, int amountOfChunks, int seed, vec3 color, int difficulty);
	~Chunk();
	void ResetPosition(int amountOfChunksInWorld);
	void Update(float deltaTime); 
	void Draw();
	void AddObjects(std::vector<Obstacle*>* mActiveObstacles, std::vector<Obstacle*>* mObstacles);
	void RemoveObjects(std::vector<Obstacle*>* mActiveObstacles);

	//----------------------------------------
	//Chunk variables
	int mZoneID = 0; 
	int mChunkID = 0;
	int mAmountOfChunks = 0;
	int mSeed = 0;
	int mPerlinY = 0;
	vec3 mColor;
	ZoneType mTypeOfZone; 
	int skippedAdded = 0;
	std::string mZoneName;
	float skippedObstacleCount=0;
	float unskippedObstacleCount=0;

	//mesh generation
	int size = 150;
	int subdivision = 50;
	int roadWidth = 12 / 2;
	const int maxAmplifacation = 30;
	const int maxRoadDist = (subdivision / 2) - roadWidth;
	const int mGroundLevel = -6;
	const int subdivisionSize = static_cast<int>(size / subdivision);

	std::vector<float> meshData;
	std::vector<unsigned int> meshIndices;
	std::vector<float> meshNormals;
	std::vector<float> verticesAmountNormals;
	std::vector<float> meshColor;

	TerrainModel* mTerrainModel;
	//---------------------------------
	//spawnpoint code
	uint spawnPointSeed;
	std::vector<SpawnPoint*> mObstacleSpawnPoints;
	const int AMOUNTOFLANES = 3;
	int density = 2;      // density of rocks around the spline
	float splineWidth = 12.0f; //6.0f
	float obstacleRadius = 12.0f; //6.0f

	float obstacleRadiusPowf = powf(obstacleRadius,2);
	void SetDifficulty(float factor);

	//----------------------------------------
private:
	void GenerateChunk();
	void GenerateSubdividedPlane();
	void GenerateMesh();
	void CalculateNormal(int i);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
	void GenerateSpawnPoints();
	void addPointIfValid(std::vector<vec2>& points, const vec2& possiblePosition, float obstacleRadiusPowf);
	float squaredDistance(const vec2& a, const vec2& b);

	
	//---------------------------------------------------------
	// Perlin noise implementation – via https://stackoverflow.com/questions/29711668/perlin-noise-generation
	int numOctaves = 2, primeIndex = 0;
	float persistence = 0.5f;
	int primes[10][3] = {
	 { 995615039, 600173719, 701464987 }, { 831731269, 162318869, 136250887 }, { 174329291, 946737083, 245679977 },
	 { 362489573, 795918041, 350777237 }, { 457025711, 880830799, 909678923 }, { 787070341, 177340217, 593320781 },
	 { 405493717, 291031019, 391950901 }, { 458904767, 676625681, 424452397 }, { 531736441, 939683957, 810651871 },
	 { 997169939, 842027887, 423882827 }
	};
	float Noise(const int i, const int x, const int y);
	float SmoothedNoise(const int i, const int x, const int y);
	float Interpolate(const float a, const float b, const float x);
	float InterpolatedNoise(const int i, const float x, const float y);
	float noise2D(const float x, const float y);


	BulletManager* mpBulletManager;
	ObjectShader* mpShaderManager;
};