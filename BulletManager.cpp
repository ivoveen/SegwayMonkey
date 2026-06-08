#include"BulletManager.h"
#include<bullet/btBulletDynamicsCommon.h>
#include "PhysicsDraw.h"

BulletManager::BulletManager() {
	BroadPhase = new btDbvtBroadphase();
	CollisionConfiguration = new btDefaultCollisionConfiguration();
	Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	ConstraintSolver = new btSequentialImpulseConstraintSolver();
	DynamicPhysicsWorld = new btDiscreteDynamicsWorld(Dispatcher, BroadPhase, ConstraintSolver, CollisionConfiguration);
	DynamicPhysicsWorld->setGravity(btVector3(0, -70, 0)); 
	//creating a custom debug drawer
	MyPhysicsDraw = new PhysicsDraw();
	DynamicPhysicsWorld->setDebugDrawer(MyPhysicsDraw);
	MyPhysicsDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

BulletManager::~BulletManager() {
    DynamicPhysicsWorld->setDebugDrawer(nullptr);
    delete MyPhysicsDraw;
    delete DynamicPhysicsWorld;
    delete ConstraintSolver;
    delete Dispatcher;
    delete CollisionConfiguration;
    delete BroadPhase;
}
