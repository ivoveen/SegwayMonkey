#pragma once
#include<bullet/btBulletDynamicsCommon.h>

class ContactCallback : public btCollisionWorld::ContactResultCallback {
public:
	btTransform playerBody;
	btTransform floorCollider;
	btTransform	leftWallCollider;
	btTransform rightWallCollider;
	bool gameOver = false;
	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override {

		gameOver = true;


		return 0; // Returning 0 ignores the physics effect of the collision
	}
};

