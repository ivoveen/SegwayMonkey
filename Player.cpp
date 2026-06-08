#include "Player.h"
#include "PhysicsObj.h"
#include "BulletManager.h"
#include<bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "Graphics.h"


Player::Player(vec3 position, BulletManager* bulletManager, ObjectShader* shaderManager,char* FileName, const std::string& shaderFilename) : GameObject::GameObject(shaderManager, FileName, shaderFilename) {
	mBulletManager = bulletManager;
	worldPosition = position;
}
Player::Player(vec3 position, ObjectShader* shaderManager, char* FileName, const std::string& shaderFilename) : GameObject::GameObject(shaderManager, FileName, shaderFilename) {
	worldPosition = position;
}

Player::~Player() {

}

void Player::UpdateShowcaseMovement(float deltaTime) {
	vec3 increase = vec3(deltaTime, deltaTime, deltaTime);
	rotations += increase;
	scales += (increase * 0.5f);
	worldPosition.y += deltaTime * 2;
}


void Player::UpdateMovement(float deltaTime, std::vector<int>* keys) {

	btRigidBody* rigidBody = MyPhysObj->GetRigidBody();
	btVector3 vel = rigidBody->getLinearVelocity();

	if (vel.getY() <= 0) {
		goingDown = true;
	}
	if (!grounded && goingDown && worldPosition.y < -4.70f) {
		grounded = true;
		goingDown = false;
		rigidBody->setFriction(1);

	}

	bool buttonpressed = false;
	for (int i = 0; i < keys->size(); i++) {

		switch (keys->at(i))
		{
		case XK_space:
			if (grounded) {
				//only jump when on the floor
				buttonpressed = true;
				grounded = false;
				goingDown = false;
				rigidBody->setFriction(0);
				rigidBody->setActivationState(ACTIVE_TAG);
				rigidBody->setLinearVelocity(btVector3(0, 30, 0));
			}
			break;
		case XK_a:
			buttonpressed = true;
			rigidBody->setActivationState(ACTIVE_TAG);
			rigidBody->setLinearVelocity(btVector3(10,vel.getY(), vel.getZ() ));
			if (rotations.y < PI / 8) {
				rigidBody->setAngularVelocity(btVector3(0, 4, 0));
			}
			else {
				rigidBody->setAngularVelocity(btVector3(0, 0, 0));
			}

			break;
		case XK_d:
			buttonpressed = true;
			rigidBody->setActivationState(ACTIVE_TAG);				
			rigidBody->setLinearVelocity(btVector3(-10, vel.getY(), vel.getZ()));
			if (rotations.y > -PI / 8) {
				rigidBody->setAngularVelocity(btVector3(0, -4, 0));
			}
			else {
				rigidBody->setAngularVelocity(btVector3(0, 0, 0));
			}
			break;
		}
	}

	if (!buttonpressed) {
		if (rotations.y < -0.1) {
			rigidBody->setAngularVelocity(btVector3(0, 4, 0));
		}
		else if (rotations.y > 0.1) {
			rigidBody->setAngularVelocity(btVector3(0, -4, 0));
		}
		else if (rotations.y < 0.1 && rotations.y > -0.1) {
			rigidBody->setAngularVelocity(btVector3(0, 0, 0));
		}

	}

	//int x = velocity.getX();
	//if (velocity.getX() != 0) {
	//	rigidBody->setLinearVelocity(btVector3(velocity.getX() - (velocity.getX() * groundFriction * deltaTime), velocity.getY(), velocity.getZ()));
	//	//printf("%f", velocity.getX());
	//}
	//
	
}