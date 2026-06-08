#pragma once
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
#include "PhysicsDraw.h"

class BulletManager
{
public:
	BulletManager();
	~BulletManager();
	PhysicsDraw* MyPhysicsDraw;
	btBroadphaseInterface* BroadPhase;
	btDefaultCollisionConfiguration* CollisionConfiguration;
	btCollisionDispatcher* Dispatcher;
	btSequentialImpulseConstraintSolver* ConstraintSolver;
	btDiscreteDynamicsWorld* DynamicPhysicsWorld = nullptr;

};

