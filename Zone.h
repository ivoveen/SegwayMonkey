
#pragma once
#include <vector>
#include <string>
#include "ZoneType.h"
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof

class Chunk;
class ObjectShader;
class BulletManager;

class Zone
{
public:
	Zone(ObjectShader* shaderManager, BulletManager* mpBulletManager, int zoneID, int difficulty);
	~Zone();
	bool GenerateZone(int difficulty);
	void GenerateName();
	void Update(float deltaTime);
	void Draw();

	//----------------------------------------
	//zone variables
	std::vector<Chunk*> mZoneChunks;
	glm::vec3 mColor;
	int mZoneID = 0;
	ZoneType mTypeOfZone;
	std::string mZoneName;
	int minimumAmountChunks = 5, maximumAmountChunks = 10;
	
	//----------------------------------------
	const float GROUNDSPEED = 0.2f;
	int frontChunk = 0;

private:

	uint seed;
	ObjectShader* mpShaderManager;
	BulletManager* mpBulletManager;
};

