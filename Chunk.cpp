#include "Chunk.h"
#include "ObjectShader.h"
#include "Obstacle.h"
#include "BulletManager.h"
#include "PhysicsObj.h"
#include "xorshift32.h"
#include <vector>
Chunk::Chunk(ObjectShader* shaderManager, BulletManager* bulletManager, ZoneType typeOfZone, int zoneID, int chunkID, int amountOfChunks, int seed, vec3 color, int difficulty) {
	mpShaderManager = shaderManager;
	mpBulletManager = bulletManager;
	mSeed = seed;
	spawnPointSeed = seed + chunkID;
	mPerlinY = chunkID * subdivision;
	mTypeOfZone = typeOfZone;
	mZoneID = zoneID;
	mChunkID = chunkID;
	mAmountOfChunks = amountOfChunks;
	mColor = color;

	splineWidth = mix(12.0f, 6.0f, (static_cast<float>(difficulty-1)/ 3.0f)); //6.0f
	obstacleRadius = mix(12.0f, 6.0f, (static_cast<float>(difficulty) / 3.0f)); //6.0f}
	GenerateChunk();
}


Chunk::~Chunk() {
	delete mTerrainModel;
}


void Chunk::GenerateChunk() {
	GenerateMesh();
	GenerateSpawnPoints();
}

void Chunk::ResetPosition(int amountOfChunksInWorld) {
	mTerrainModel->worldPosition = glm::vec3(0.0f, 0.0f, static_cast<float>(size * amountOfChunksInWorld));
}

void Chunk::GenerateMesh() {
	GenerateSubdividedPlane();
	int index = 0;
	float noiseValue;
	float roadDistance = 0;
	int storeSeed = 0;
	for (int j = 0; j <= subdivision; j++) {
		for (int i = 0; i <= subdivision; i++) {
			index = (i + (j * (subdivision + 1))) * 3;
			if (i > static_cast<int>(static_cast<float>(subdivision) * 0.5f) - roadWidth && i < static_cast<int>(static_cast<float>(subdivision) * 0.5f) + roadWidth) {
				//the road
				meshData[index + 1] = static_cast<float>(mGroundLevel);
				//color
				meshColor.push_back(131.0f / 255.0f);
				meshColor.push_back(101.0f / 255.0f);
				meshColor.push_back(57.0f / 255.0f);
			}
			else {
				//the terrain
				roadDistance = static_cast<float>(min(abs(i - static_cast<int>(static_cast<float>(subdivision) * 0.5f) - roadWidth), abs(i - static_cast<int>(static_cast<float>(subdivision) * 0.5f) + roadWidth)));
				float amplify = powf(roadDistance / static_cast<float>(maxRoadDist), 2);

				if ((j == 0 && mChunkID == 0) || (j == subdivision && mChunkID == mAmountOfChunks)) {
					storeSeed = mSeed;
					mSeed = 0;
					noiseValue = noise2D(static_cast<float>(i), static_cast<float>(0 - mPerlinY));

					mSeed = storeSeed;
				}
				else {
					noiseValue = noise2D(static_cast<float>(i), static_cast<float>(j));

				}
				meshColor.push_back(mColor.x);
				meshColor.push_back(mColor.y);
				meshColor.push_back(mColor.z);


				//amplify things far away and decrease things close to the road
				noiseValue += (amplify * static_cast<float>(maxAmplifacation));

				if (roadDistance < 4) {
					meshData[index + 1] = static_cast<float>(mix(static_cast<int>(noiseValue), -6, amplify));

				}
				else {
					meshData[index + 1] = noiseValue;
				}
				//color

			}
		}
	}
	//calculate normals
	for (int i = 0; i < static_cast<int>(meshIndices.size()); i += 3) {
		CalculateNormal(i);
	}
	//average vertex normals
	for (size_t i = 0; i < meshData.size(); i += 3) {
		vec3 normal = vec3(meshNormals[i], meshNormals[i + 1], meshNormals[i + 2]);
		normal = normal / verticesAmountNormals[i / 3];
		meshNormals[i] = normal.x;
		meshNormals[i + 1] = normal.y;
		meshNormals[i + 2] = normal.z;
	}

	/*for (size_t i = 0; i < meshColor.size(); i += 3) {
		printf("size: %i", meshColor.size() / 3);
		printf(" % f, % f, % f \n", meshColor[i], meshColor[i + 1], meshColor[i + 2]);
	}*/
	/*for (size_t i = 0; i < meshIndices.size(); i += 3) {
		printf("size: %i", meshIndices.size() / 3);
		printf("Indeces:  % i, % i, % i \n", meshIndices[i], meshIndices[i + 1], meshIndices[i + 2]);
	}*/
	/*for (size_t i = 0; i < meshNormals.size(); i += 3) {
		printf("size: %i", (meshNormals.size() / 3));
		printf("Normals:  % f, % f, % f\n", meshNormals[i], meshNormals[i + 1], meshNormals[i + 2]);
	}*/

	mTerrainModel = new TerrainModel(mpShaderManager, meshData, meshIndices, meshNormals, meshColor);
}

void Chunk::CalculateNormal(int i) {
	vec3 trianglep1 = vec3(meshData[(meshIndices[i] * 3)], meshData[(meshIndices[i] * 3) + 1], meshData[(meshIndices[i] * 3) + 2]);
	vec3 trianglep2 = vec3(meshData[(meshIndices[i + 1] * 3)], meshData[(meshIndices[i + 1] * 3) + 1], meshData[(meshIndices[i + 1] * 3) + 2]);
	vec3 trianglep3 = vec3(meshData[(meshIndices[i + 2] * 3)], meshData[(meshIndices[i + 2] * 3) + 1], meshData[(meshIndices[i + 2] * 3) + 2]);

	vec3 vecU = trianglep2 - trianglep1;
	vec3 vecV = trianglep3 - trianglep1;

	vec3 normal;
	normal = cross(vecV, vecU);
	normal = normalize(normal);
	//add the face normal to all related vertices and add one more face to the normal count.
	meshNormals[(meshIndices[i] * 3)] += normal.x;
	meshNormals[(meshIndices[i] * 3) + 1] += normal.y;
	meshNormals[(meshIndices[i] * 3) + 2] += normal.z;
	verticesAmountNormals[meshIndices[i]]++;

	meshNormals[(meshIndices[i + 1] * 3)] += normal.x;
	meshNormals[(meshIndices[i + 1] * 3) + 1] += normal.y;
	meshNormals[(meshIndices[i + 1] * 3) + 2] += normal.z;
	verticesAmountNormals[meshIndices[i + 1]]++;

	meshNormals[(meshIndices[i + 2] * 3)] += normal.x;
	meshNormals[(meshIndices[i + 2] * 3) + 1] += normal.y;
	meshNormals[(meshIndices[i + 2] * 3) + 2] += normal.z;
	verticesAmountNormals[meshIndices[i + 2]]++;
}

void Chunk::GenerateSubdividedPlane() {
	float step = static_cast<float>(size) / static_cast<float>(subdivision);
	for (int j = 0; j <= subdivision; j++) {
		for (int i = 0; i <= subdivision; i++) {
			meshData.push_back(static_cast<float>(i) * step);
			meshData.push_back(0.0f);
			meshData.push_back(static_cast<float>(j) * step);

			meshNormals.push_back(0.0f);
			meshNormals.push_back(0.0f);
			meshNormals.push_back(0.0f);

			verticesAmountNormals.push_back(0);
		}
	}
	for (int j = 0; j < static_cast<float>(subdivision); j++) {
		for (int i = 0; i < static_cast<float>(subdivision); i++) {
			//first triangle
			meshIndices.push_back(static_cast<unsigned int>(i + (j *subdivision) + j));
			meshIndices.push_back(static_cast<unsigned int>(i + (j * subdivision) + j) + 1.0f);
			meshIndices.push_back(static_cast<unsigned int>(i + (j *subdivision) + j) + static_cast<float>(subdivision) + 1.0f + 1.0f);

			//second triangle
			meshIndices.push_back(static_cast<unsigned int>(i + (j * subdivision) + j) + static_cast<float>(subdivision) + 1.0f + 1.0f);
			meshIndices.push_back(static_cast<unsigned int>(i + (j * subdivision) + j) + static_cast<float>(subdivision) + 1.0f);
			meshIndices.push_back(static_cast<unsigned int>(i + (j * subdivision) + j));
		}
	}
}


// This perlin noise implementation was taken from the procedural generation lecture Jacco gave.
// Perlin noise implementation – via https://stackoverflow.com/questions/29711668/perlin-noise-generation
float Chunk::Noise(const int i, const int x, const int y)
{
	int n = x + y * 57 + mSeed; n = (n << 13) ^ n;
	const int a = primes[i][0], b = primes[i][1], c = primes[i][2];
	const int t = (n * (n * n * a + b) + c) & 0x7fffffff;
	return 1.0f - (float)t / 1073741824.0f;
}
float Chunk::SmoothedNoise(const int i, const int x, const int y)
{
	const float corners = (Noise(i, x - 1, y - 1) + Noise(i, x + 1, y - 1) + Noise(i, x - 1, y + 1) + Noise(i, x + 1, y + 1)) / 16;
	const float sides = (Noise(i, x - 1, y) + Noise(i, x + 1, y) + Noise(i, x, y - 1) + Noise(i, x, y + 1)) / 8;
	const float center = Noise(i, x, y) / 4;
	return corners + sides + center;
}
float Chunk::Interpolate(const float a, const float b, const float x)
{
	const float ft = x * 3.1415927f, f = (1 - cosf(ft)) * 0.5f;
	return a * (1 - f) + b * f;
}
float Chunk::InterpolatedNoise(const int i, const float x, const float y)
{
	const int integer_X = (int)x, integer_Y = (int)y;
	const float fractional_X = x - (float)integer_X, fractional_Y = y - (float)integer_Y;
	const float v1 = SmoothedNoise(i, integer_X, integer_Y);
	const float v2 = SmoothedNoise(i, integer_X + 1, integer_Y);
	const float v3 = SmoothedNoise(i, integer_X, integer_Y + 1);
	const float v4 = SmoothedNoise(i, integer_X + 1, integer_Y + 1);
	const float i1 = Interpolate(v1, v2, fractional_X);
	const float i2 = Interpolate(v3, v4, fractional_X);
	return Interpolate(i1, i2, fractional_Y);
}
float Chunk::noise2D(const float x, float y)
{
	y += static_cast<float>(mPerlinY);
	float total = 0.0f, frequency = (float)(2 << numOctaves), amplitude = 100.0f;
	for (int i = 0; i < numOctaves; ++i)
	{
		frequency /= 2, amplitude *= persistence;
		total += InterpolatedNoise(static_cast<int>((primeIndex + i) % 10), x / frequency, y / frequency) * amplitude;
	}
	return total / frequency;
}

void Chunk::GenerateSpawnPoints() {
	//-----------------------------------------------
	//select road points
	float leftRoadX = 95, rightRoadX = 62;
	float roadWidth = abs(leftRoadX - rightRoadX);
	float laneWidth = roadWidth / static_cast<float>(AMOUNTOFLANES + 2);
	std::vector<vec2> points;
	for (float j = 0; j < static_cast<float>(size); j += laneWidth * 3.0f) {
		spawnPointSeed = RandomUInt(spawnPointSeed);
		int lane = 1 + (spawnPointSeed % 3);
		vec2 point = vec2((static_cast<float>(lane) * laneWidth) + rightRoadX, j);//choose points on a square grid on the road
		points.push_back(point);
	}

	//-----------------------------------------------
	//Catmull-Rom spline
	std::vector<vec2> splinePoints;
	vec2 point1, point2, point3, point4;
	vec2 A, B, C, D;
	for (float i = 1; i < static_cast<float>(points.size()) - 2.0f; i++) {
		point1 = points[i - 1.0f];
		point2 = points[i];
		point3 = points[i + 1.0f];
		point4 = points[i + 2.0f];

		for (float t = 0.0f; t <= 1.0f; t += 0.1f) {
			A = (-point1 + 3.0f * point2 - 3.0f * point3 + point4);
			B = (2.0f * point1 - 5.0f * point2 + 4.0f * point3 - point4);
			C = (-point1 + point3);
			D = 2.0f * point2;

			vec2 result = (A * powf(t, 3) + B * powf(t, 2) + C * t + D) / 2.0f;
			splinePoints.push_back(result);
		}
	}
	points.clear();
	//-----------------------------------------------
	//get positions outside the spline.
	for (int i = 0; i < static_cast<int>(splinePoints.size()) - 1; i += density) {
		vec2 tangent = normalize(splinePoints[i + 1] - splinePoints[i]);

		// Calculate perpendicular vectors for both sides
		vec2 perpendicularLeft = vec2(tangent.y, -tangent.x);
		vec2 perpendicularRight = vec2(-tangent.y, tangent.x);

		vec2 possiblePositionLeft = vec2(leftRoadX - 1.0f, 0);
		vec2 possiblePositionRight = vec2(rightRoadX + 1.0f, 0);
		for (float j = 0; possiblePositionLeft.x < leftRoadX && possiblePositionRight.x > rightRoadX; j++) {
			// Calculate possible positions on both sides
			possiblePositionLeft = perpendicularLeft * (splineWidth + (obstacleRadius * j)) + splinePoints[i];
			possiblePositionRight = perpendicularRight * (splineWidth + (obstacleRadius * j)) + splinePoints[i];

			// Add points if they are valid
			if (possiblePositionLeft.x < leftRoadX) addPointIfValid(points, possiblePositionLeft, obstacleRadiusPowf);
			if (possiblePositionLeft.x > rightRoadX) addPointIfValid(points, possiblePositionRight, obstacleRadiusPowf);
		}
	}
	//-----------------------------------------------
	// create spawnPoint objects;
	for (int i = 0; i < static_cast<int>(points.size()); i++) {
		SpawnPoint* mpSpawnPoint = new SpawnPoint;
		vec3 pos = vec3(points[i].x, 0, points[i].y);
		mpSpawnPoint->chunkPosition = pos;
		if ((i % 5) == 0) {
			mpSpawnPoint->enabled = false;
			skippedObstacleCount++;
		}
		else {
			mpSpawnPoint->enabled = true;
		}
		mObstacleSpawnPoints.push_back(mpSpawnPoint);
	}

}

void Chunk::addPointIfValid(std::vector<vec2>& points, const vec2& possiblePosition, float obstacleRadiusPowf) {
	if (points.size() == 0) {
		points.push_back(possiblePosition);
		return;
	}

	float smallestDist = 9999;
	for (int j = 0; j < static_cast<int>(points.size()); j++) {
		float distPowf = squaredDistance(points[j], possiblePosition);
		if (distPowf < smallestDist) {
			smallestDist = distPowf;
		}
	}

	if (smallestDist > obstacleRadiusPowf) {
		points.push_back(possiblePosition);
	}
}


float Chunk::squaredDistance(const vec2& a, const vec2& b) {
	return powf((a.x - b.x), 2) + powf((a.y - b.y), 2);
}

void Chunk::SetDifficulty(float factor) {
	skippedAdded = static_cast<int>(floor(factor * skippedObstacleCount));
	for (int i = 0; i < skippedAdded; i++) {
		mObstacleSpawnPoints[i * 5]->enabled = true;
	}
}

void Chunk::Update(float deltaTime) {
	mTerrainModel->Update();

}

void Chunk::Draw() {
	mpShaderManager->use(ObjectShader::Terrain);
	mTerrainModel->Draw();
}

void Chunk::AddObjects(std::vector<Obstacle*>* activeObstacles, std::vector<Obstacle*>* obstacles) {
	int i = 0;
	std::vector<Obstacle*>& mObstacles = (*obstacles);
	std::vector<Obstacle*>& mActiveObstacles = (*activeObstacles);

	int activatedObstacles = 0;
	while (activatedObstacles < static_cast<int>(mObstacleSpawnPoints.size())) {
		if (!mObstacles[i]->active) {
			//activate and init 
			mObstacles[i]->active = true;
			mObstacles[i]->mChunkID = mChunkID;
			mObstacles[i]->mZoneID = mZoneID;

			//skip unenabled spawnpoints, needed for difficulty increase.

			vec3 pos = mObstacleSpawnPoints[activatedObstacles]->chunkPosition + mTerrainModel->worldPosition;
			mObstacles[i]->MyPhysObj->SetPosition(btVector3(pos.x, pos.y, pos.z));
			mpBulletManager->DynamicPhysicsWorld->addRigidBody(mObstacles[i]->MyPhysObj->GetRigidBody());

			mActiveObstacles.push_back(mObstacles[i]);
			//save where in the list this obstacle is
			//mObstacleSpawnPoints[activatedObstacles]->activeIndex = i;
			//next
			if (mObstacleSpawnPoints[activatedObstacles]->enabled == false)activatedObstacles++;
			activatedObstacles++;
		}
		i++;
	}
}
void Chunk::RemoveObjects(std::vector<Obstacle*>* activeObstacles) {
	std::vector<Obstacle*>& mActiveObstacles = (*activeObstacles);

	int i = 0;
	int deactivatedObstacles = 0;
	while (deactivatedObstacles < static_cast<int>(mObstacleSpawnPoints.size())) {
		if (mActiveObstacles[i]->active && mActiveObstacles[i]->mChunkID == mChunkID && mActiveObstacles[i]->mZoneID == mZoneID) {
			//activate and init 
			mActiveObstacles[i]->active = false;
			mpBulletManager->DynamicPhysicsWorld->removeRigidBody(mActiveObstacles[i]->MyPhysObj->GetRigidBody());

			mActiveObstacles.erase(mActiveObstacles.begin() + i);

			if (mObstacleSpawnPoints[deactivatedObstacles]->enabled == false)deactivatedObstacles++;
			deactivatedObstacles++;

		}
		else {
			i++;
		}
	}
}