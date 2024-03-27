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
typedef struct btRigidBodyConstructionInfo btRigidBodyConstructionInfo;
typedef struct btRigidBody btRigidBody;

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
void btDiscreteDynamicsWorld_addRigidBody(btDiscreteDynamicsWorld *dynamicsWorld, void *body);
void btDiscreteDynamicsWorld_stepSimulation(btDiscreteDynamicsWorld *dynamicsWorld, float timeStep, int maxSubSteps);

btBoxShape *btBoxShape_new(vec3 boxHalfExtents);
void btCollisionShape_calculateLocalInertia(void *collisionShape, float mass, vec3 inertia);

btTransform *btTransform_new(void);
void btTransform_setIdentity(btTransform *transform);
void btTransform_setOrigin(btTransform *transform, vec3 origin);
void btTransform_getOrigin(btTransform *transform, vec3 origin);
void btTransform_getRotation(btTransform *transform, versor rotation);

btDefaultMotionState *btDefaultMotionState_new(btTransform *startTrans);
btRigidBodyConstructionInfo *btRigidBodyConstructionInfo_new(float mass, void *motionState, void *collisionShape, vec3 localInertia);

btRigidBody *btRigidBody_new(btRigidBodyConstructionInfo *constructionInfo);
btTransform *btRigidBody_getWorldTransform(void *body);
void btRigidBody_applyCentralForce(void *body, vec3 force);
void btRigidBody_applyTorque(void *body, vec3 torque);
void btRigidBody_setSleepingThresholds(void *body, float linear, float angular);
void btRigidBody_activate(void *body);

#endif
