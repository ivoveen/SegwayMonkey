#include "WorldManager.h"
#include "GameObject.h"
#include "PhysicsObj.h"
#include "Zone.h"
#include "BulletManager.h"
#include "Chunk.h"
#include "xorshift32.h"
#include "Obstacle.h"
#include "ContactCallback.h"
#include "Player.h"

WorldManager::WorldManager(ObjectShader* shaderManager, BulletManager* bulletManager, int difficulty, Player* player) {
	mpShaderManager = shaderManager;
	mpBulletManager = bulletManager;
	mPlayer = player;
	seed = static_cast<uint>(time(NULL));
	mDifficulty = difficulty;

	GenerateWorld(difficulty);
	GenerateObjects();
	mSelectedChunkID = AMOUNTOFCHUNKSINTHEWORLD;
	mSelectedZoneID = 0;
	for (int i = 0; i < AMOUNTOFCHUNKSINTHEWORLD; i++) {
		chunksInTheWorld.push_back(mWorldZones[mSelectedZoneID]->mZoneChunks[i]);
		if(i == 0)chunksInTheWorld[i]->ResetPosition(-2);
		else chunksInTheWorld[i]->ResetPosition(i);

		chunksInTheWorld[i]->AddObjects(&mActiveObstacles, &mObstacles);
		chunksInTheWorld[i]->ResetPosition(i);
	}
}

WorldManager::~WorldManager() {
	for (int i = 0; i < amountOfZones; i++) {
		delete mWorldZones[i];
	}
	for (int i = 0; i < mObstacles.size(); i++) {
		delete mObstacles[i];
	}

}

void WorldManager::GenerateWorld(int difficulty) {
	for (int i = 0; i < amountOfZones; i++) {
		mWorldZones.push_back(new Zone(mpShaderManager, mpBulletManager, i, difficulty));
	}

	CreateHitboxes(vec3(50, 8, 50.0f), vec3(40.0f, 20.0f, 2.2f));
	CreateHitboxes(vec3(73, -8, -10.0f), vec3(40.0, 2.0f, 10.0f));
	CreateHitboxes(vec3(101.0f, 3.9f, 50.0f), vec3(40.0f, 20.0f, 2.2f));
}

void WorldManager::CreateHitboxes(vec3 Pos, vec3 scale) {
	GameObject* mGroundGameObject;
	mGroundGameObject = new GameObject(mpShaderManager, (char*)"./Resources/Models/pitfallgroundhitbox.obj", "Object");
	mGroundGameObject->scales = scale;
	mGroundGameObject->SetPosition(Pos);
	mGroundGameObject->SetYRotation(-PI / 2);
	mGroundGameObject->CreatePhysicsObj(mpBulletManager, mGroundGameObject->CreateMyShape(ObjectModel::BOX), 1);
	btRigidBody* mBody = mGroundGameObject->MyPhysObj->GetRigidBody();
	mBody->activate();
	mBody->setGravity(btVector3(0, 0, 0));
	mBody->setLinearFactor(btVector3(0, 0, 0));
	mBody->setAngularFactor(btVector3(0, 0, 0));
	mBody->setDamping(0.0, 0.0);
	groundHitboxes.push_back(mGroundGameObject);
}

void WorldManager::SyncDifficulty() {
	//difficultyFactor;
	mSpeed =  mix(0.2f, 0.4f, difficultyFactor);
}

void WorldManager::Update(float deltaTime) {
	SyncDifficulty();
	for (int i = 0; i < mActiveObstacles.size(); i++) {
		mpBulletManager->DynamicPhysicsWorld->contactPairTest(mPlayer->MyPhysObj->GetRigidBody(), mActiveObstacles[i]->MyPhysObj->GetRigidBody(), collisionCallback);
	}


	int chunkSize = chunksInTheWorld[0]->size;
	vec3 chunkPosition, prevChunkPosition;
	float addedPos = mSpeed * deltaTime * 100;

	//update score
	mScore += addedPos * difficultyFactor * mDifficulty;


	for (int i = 0; i < AMOUNTOFCHUNKSINTHEWORLD; i++) {
		chunkPosition = chunksInTheWorld[i]->mTerrainModel->GetPosition();
		chunkPosition.z -= addedPos;

		//is the chunk behind the player?
		if (chunkPosition.z <= 0 - chunksInTheWorld[i]->size) {
			//remove old chunk
			chunksInTheWorld[i]->RemoveObjects(&mActiveObstacles);
			
			//get the next chunk
			chunksInTheWorld[i] = mWorldZones[mSelectedZoneID]->mZoneChunks[mSelectedChunkID];

			//set up the new chunk.
			chunksInTheWorld[i]->SetDifficulty(difficultyFactor);
			chunksInTheWorld[i]->ResetPosition(AMOUNTOFCHUNKSINTHEWORLD - 1);
			chunkPosition = chunksInTheWorld[i]->mTerrainModel->GetPosition();
			
			//make sure there is no gap between chunks
			if (i == 0) {
				prevChunkPosition = chunksInTheWorld[AMOUNTOFCHUNKSINTHEWORLD - 1]->mTerrainModel->GetPosition();
				prevChunkPosition.z -= addedPos;
			}
			else prevChunkPosition = chunksInTheWorld[i - 1]->mTerrainModel->GetPosition();
			chunkPosition.z = prevChunkPosition.z + chunkSize;

			//add objects to the chunk
			chunksInTheWorld[i]->AddObjects(&mActiveObstacles, &mObstacles);

			//increase difficulty
			if(difficultyFactor < (1.0f - 0.03f))	difficultyFactor += 0.03f;

			//prepare for the next new chunk
			mSelectedChunkID++;
			if (mSelectedChunkID == 3) mCurrentZoneID = mSelectedChunkID; //currentZone is the zone the player is still in
			if (mSelectedChunkID >= mWorldZones[mSelectedZoneID]->mZoneChunks.size()) {
				//end of zone has been reached.
				seed = RandomUInt(seed);
				mSelectedZoneID = (seed % (amountOfZones));//select random new zone
				mSelectedChunkID = 0;
			}
		}
		


		chunksInTheWorld[i]->mTerrainModel->SetPosition(chunkPosition);
		chunksInTheWorld[i]->Update(deltaTime);

	}

	for (int i = 0; i < mActiveObstacles.size(); i++) {
		btRigidBody* rigidBody = mActiveObstacles[i]->MyPhysObj->GetRigidBody();
		rigidBody->setLinearVelocity(btVector3(0,0,-mSpeed *100));
		mActiveObstacles[i]->Update();
	}
}

void WorldManager::Draw() {
	for (int i = 0; i < AMOUNTOFCHUNKSINTHEWORLD; i++) {
		chunksInTheWorld[i]->Draw();
	}
}

void WorldManager::GenerateObjects() {
	objectSeed = static_cast<uint>(time(NULL));
	
	char* filePath = 0;
	for (int i = 0; i < 300; i++) {
		objectSeed = RandomUInt(objectSeed);
		int rockNumber = objectSeed % 5;
		switch (rockNumber) {
		case 0:
			filePath = (char*)"./Resources/Models/rock1.obj";
			break;
		case 1:
			filePath = (char*)"./Resources/Models/rock3.obj";

			break;
		case 2:
			filePath = (char*)"./Resources/Models/rock4.obj";

			break;
		case 3:
			filePath = (char*)"./Resources/Models/rock5.obj";

			break;
		case 4:
			filePath = (char*)"./Resources/Models/rock6.obj";

			break;
		}

		vec3 pos = vec3(77.5f, 0.0f, 5.0f * i);
		Obstacle* mObstacle = new Obstacle(pos, mpBulletManager, mpShaderManager, filePath, "Object");
		mObstacle->scales = glm::vec3(0.025f, 0.025f, 0.025f);
		mObstacle->CreatePhysicsObj(mpBulletManager, mObstacle->CreateMyShape(ObjectModel::CONVEXHULL), 1);
		mObstacle->MyPhysObj->GetRigidBody()->setAngularFactor(btVector3(0, 0, 0));
		mpBulletManager->DynamicPhysicsWorld->removeRigidBody(mObstacle->MyPhysObj->GetRigidBody());
		mObstacles.push_back(mObstacle);
	}
}


void WorldManager::DrawObjects() {
	for (int i = 0; i < mActiveObstacles.size(); i++) {
		mActiveObstacles[i]->Draw();
	}
}
