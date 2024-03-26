#ifndef ILAPIN_PHYSICS_H
#define ILAPIN_PHYSICS_H

#include "bullet_physics.h"
#include <cglm/cglm.h>
#include <glib.h>
#include "transform.h"

typedef struct DynamicsWorld {
	btDefaultCollisionConfiguration *collision_configuration;
	btCollisionDispatcher *dispatcher;
	btBroadphaseInterface *overlapping_pair_cache;
	btSequentialImpulseConstraintSolver *solver;
	btDiscreteDynamicsWorld *dynamics_world;
} DynamicsWorld;

typedef struct BoxRigidBody {
	btBoxShape *shape;
	btTransform *transform;
	btDefaultMotionState *motion_state;
	btRigidBodyConstructionInfo *rb_info;
	btRigidBody *rigid_body;
} BoxRigidBody;

typedef struct PhysicsToGraphicsTransformLink {
	btTransform *bt_transform;
	Transform *transform;
} PhysicsToGraphicsTransformLink;

DynamicsWorld *init_dynamic_world(void);
BoxRigidBody *create_box_rigid_body(float mass, vec3 half_extents, vec3 position/*, versor rotation*/);
void sync_physics_and_graphics_transforms(GArray *links);

#endif
