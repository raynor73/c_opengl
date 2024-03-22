#include <iostream>
#include <bullet/btBulletDynamicsCommon.h>
#include <cglm/cglm.h>

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

}
