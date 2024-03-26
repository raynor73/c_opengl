#include "physics.h"
#include <stdlib.h>
#include "logger.h"

DynamicsWorld *init_dynamic_world(void) {
	DynamicsWorld *dynamics_world = (DynamicsWorld *) malloc(sizeof(DynamicsWorld));
	if (!dynamics_world) {
		error("Failed to allocate memory for Dynamics World\n");
	}
	
	dynamics_world->collision_configuration = btDefaultCollisionConfiguration_new();
	dynamics_world->dispatcher = btCollisionDispatcher_new(dynamics_world->collision_configuration);
	dynamics_world->overlapping_pair_cache = btDbvtBroadphase_new();
	dynamics_world->solver = btSequentialImpulseConstraintSolver_new();
	dynamics_world->dynamics_world = btDiscreteDynamicsWorld_new(
		dynamics_world->dispatcher, 
		dynamics_world->overlapping_pair_cache, 
		dynamics_world->solver, 
		dynamics_world->collision_configuration
	);
	
	return dynamics_world;
}

BoxRigidBody *create_box_rigid_body(float mass, vec3 half_extents, vec3 position/*, versor rotation*/) {
	BoxRigidBody *rigid_body = (BoxRigidBody *) malloc(sizeof(BoxRigidBody));
	if (!rigid_body) {
		error("Faild to allocate memory for Box Rigid Body");
	}
	
	rigid_body->shape = btBoxShape_new(half_extents);
	rigid_body->transform = btTransform_new();
	btTransform_setIdentity(rigid_body->transform);
	btTransform_setOrigin(rigid_body->transform, position);
	
	rigid_body->motion_state = btDefaultMotionState_new(rigid_body->transform);
	vec3 local_inertia = { 0, 0, 0 };
	btCollisionShape_calculateLocalInertia(rigid_body->shape, mass, local_inertia);
	rigid_body->rb_info = btRigidBodyConstructionInfo_new(
		mass, 
		rigid_body->motion_state, 
		rigid_body->shape,
		local_inertia
	);
	rigid_body->rigid_body = btRigidBody_new(rigid_body->rb_info);
	
	return rigid_body;
}

void sync_physics_and_graphics_transforms(GArray *links) {
	for (int i = 0; i < links->len; i++) {
		PhysicsToGraphicsTransformLink *link = (PhysicsToGraphicsTransformLink *) &links->data[i * sizeof(PhysicsToGraphicsTransformLink)];
		btTransform_getOrigin(link->bt_transform, link->transform->position);		
		btTransform_getRotation(link->bt_transform, link->transform->rotation);
	}
}
