#include <iostream>
#include <bullet/btBulletDynamicsCommon.h>
#include <cglm/cglm.h>

struct btRigidBodyConstructionInfo;

extern "C" {
	btDefaultCollisionConfiguration *btDefaultCollisionConfiguration_new(void) {
		return new btDefaultCollisionConfiguration();
	}
	
	btCollisionDispatcher *btCollisionDispatcher_new(btDefaultCollisionConfiguration *collisionConfiguration) {
		return new btCollisionDispatcher(collisionConfiguration);
	}
	
	btBroadphaseInterface *btDbvtBroadphase_new(void) {
		return new btDbvtBroadphase();
	}
	
	btSequentialImpulseConstraintSolver *btSequentialImpulseConstraintSolver_new(void) {
		return new btSequentialImpulseConstraintSolver();
	}
	
	btDiscreteDynamicsWorld *btDiscreteDynamicsWorld_new(
		btCollisionDispatcher *dispatcher,
		btBroadphaseInterface *overlappingPairCache,
		btSequentialImpulseConstraintSolver *solver,
		btDefaultCollisionConfiguration *collisionConfiguration
	) {
		return new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	}

	void btDiscreteDynamicsWorld_setGravity(btDiscreteDynamicsWorld *dynamicsWorld, vec3 gravity) {
		dynamicsWorld->setGravity(btVector3(gravity[0], gravity[1], gravity[2]));
	}
	
	void btDiscreteDynamicsWorld_addRigidBody(btDiscreteDynamicsWorld *dynamicsWorld, void *body) {
		dynamicsWorld->addRigidBody(reinterpret_cast<btRigidBody *>(body));
	}
	
	void btDiscreteDynamicsWorld_stepSimulation(btDiscreteDynamicsWorld *dynamicsWorld, float timeStep, int maxSubSteps) {
		dynamicsWorld->stepSimulation(timeStep, maxSubSteps);
	}

	btBoxShape *btBoxShape_new(vec3 boxHalfExtents) {
		return new btBoxShape(btVector3(boxHalfExtents[0], boxHalfExtents[1], boxHalfExtents[2]));
	}
	
	btTransform *btTransform_new(void) {
		return new btTransform();
	}
	
	void btTransform_setIdentity(btTransform *transform) {
		transform->setIdentity();
	}
	
	void btTransform_setOrigin(btTransform *transform, vec3 origin) {
		transform->setOrigin(btVector3(origin[0], origin[1], origin[2]));
	}
	
	btDefaultMotionState *btDefaultMotionState_new(btTransform *startTrans) {
		return new btDefaultMotionState(*startTrans);
	}
	
	btRigidBodyConstructionInfo *btRigidBodyConstructionInfo_new(float mass, void *motionState, void *collisionShape, vec3 localInertia) {
		return reinterpret_cast<btRigidBodyConstructionInfo *>(new btRigidBody::btRigidBodyConstructionInfo(mass, reinterpret_cast<btMotionState *>(motionState), reinterpret_cast<btCollisionShape *>(collisionShape), btVector3(localInertia[0], localInertia[1], localInertia[2])));
	}
	
	btRigidBody *btRigidBody_new(btRigidBodyConstructionInfo *constructionInfo) {
		return new btRigidBody(*reinterpret_cast<btRigidBody::btRigidBodyConstructionInfo *>(constructionInfo));
	}
}
