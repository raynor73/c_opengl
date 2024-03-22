#ifndef ILAPIN_BULLET_PHYSICS_H
#define ILAPIN_BULLET_PHYSICS_H

#include <cglm/cglm.h>

typedef struct btDefaultCollisionConfiguration btDefaultCollisionConfiguration;
typedef struct btCollisionDispatcher btCollisionDispatcher;
typedef struct btBroadphaseInterface btBroadphaseInterface;
typedef struct btSequentialImpulseConstraintSolver btSequentialImpulseConstraintSolver;
typedef struct btDiscreteDynamicsWorld btDiscreteDynamicsWorld;
typedef struct btBoxShape btBoxShape;
typedef struct btTransform btTransform;
typedef struct btDefaultMotionState btDefaultMotionState;

btDefaultCollisionConfiguration *btDefaultCollisionConfiguration_new(void);
btCollisionDispatcher *btCollisionDispatcher_new(btDefaultCollisionConfiguration *collisionConfiguration);
btBroadphaseInterface *btDbvtBroadphase_new(void);
btSequentialImpulseConstraintSolver *btSequentialImpulseConstraintSolver_new(void);
btDiscreteDynamicsWorld *btDiscreteDynamicsWorld_new(
	btCollisionDispatcher *dispatcher,
	btBroadphaseInterface *overlappingPairCache,
	btSequentialImpulseConstraintSolver *solver,
	btDefaultCollisionConfiguration *collisionConfiguration
);
void btDiscreteDynamicsWorld_setGravity(btDiscreteDynamicsWorld *dynamicsWorld, vec3 gravity);
btBoxShape *btBoxShape_new(vec3 boxHalfExtents);

btTransform *btTransform_new(void);
void btTransform_setIdentity(btTransform *transform);
void btTransform_setOrigin(btTransform *transform, vec3 origin);

#endif
