

#include "PhysicsObj.h"



PhysicsObj::PhysicsObj(btCollisionShape* Shape,
	float mass,
	const btVector3& Position,  // these have defaults if needed
	const btQuaternion& Rotation)
{/*
(1 0 0 0) - Identity quaternion, no rotation,
(0 1 0 0) - 180° turn around X axis,
(0 0 1 0) - 180° turn around Y axis,
(0 0 0 1) - 180° turn around Z axis,
 */

 // We have to create a startup transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Position);
	transform.setRotation(Rotation);

	// This new info needs to create the Game Values
	m_Status = new GameValues(transform);
	m_Shape = Shape;

	// set up inertia, start with 0, and test if we have an infinite mass(0.0), if so, no inertia is possible
	btVector3 Inertia(0, 0, 0);

	if (mass != 0.0f)
		m_Shape->calculateLocalInertia(mass, Inertia);

	 
	//make a new RB with this info
	btRigidBody::btRigidBodyConstructionInfo RBinfo(mass, m_Status, m_Shape, Inertia);
	m_Body = new btRigidBody(RBinfo);
}

PhysicsObj::~PhysicsObj()
{
	// remove the things we made
	
	delete m_Body;
	delete m_Shape;
	delete m_Status;

}



