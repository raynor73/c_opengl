#include "space_box_controller.h"
#include "logger.h"
#include <stdbool.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include "constants.h"

typedef struct SpaceBoxController {
	bool is_w_key_pressed;
	bool is_s_key_pressed;
	bool is_a_key_pressed;
	bool is_d_key_pressed;
	bool is_q_key_pressed;
	bool is_e_key_pressed;
	bool is_z_key_pressed;
	bool is_c_key_pressed;
	bool is_r_key_pressed;
	bool is_f_key_pressed;
	bool is_end_key_pressed;
	
	btRigidBody *rigid_body;
} SpaceBoxController;

SpaceBoxController *space_box_controller_new(btRigidBody *rigid_body) {
	SpaceBoxController *controller = (SpaceBoxController *) malloc(sizeof(SpaceBoxController));
	if (!controller) {
		error("Failed to allocate memory for Space Box Controller\n");
	}
	
	memset(controller, 0, sizeof(SpaceBoxController));
	
	controller->rigid_body = rigid_body;
	
	return controller;
}

void space_box_controller_on_key_event(SpaceBoxController *controller, int key, int scancode, int action, int mods) {
	switch (key) {
		case GLFW_KEY_END:
			if (action == GLFW_PRESS) {
				controller->is_end_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_end_key_pressed = false;
			}
			break;
		
		// region WSAD
		case GLFW_KEY_W:
			if (action == GLFW_PRESS) {
				controller->is_w_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_w_key_pressed = false;
			}
			break;

		case GLFW_KEY_S:
			if (action == GLFW_PRESS) {
				controller->is_s_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_s_key_pressed = false;
			}
			break;

		case GLFW_KEY_A:
			if (action == GLFW_PRESS) {
				controller->is_a_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_a_key_pressed = false;
			}
			break;

		case GLFW_KEY_D:
			if (action == GLFW_PRESS) {
				controller->is_d_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_d_key_pressed = false;
			}
			break;
		// endregion
			
		// region QEZC
		case GLFW_KEY_Q:
			if (action == GLFW_PRESS) {
				controller->is_q_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_q_key_pressed = false;
			}
			break;

		case GLFW_KEY_E:
			if (action == GLFW_PRESS) {
				controller->is_e_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_e_key_pressed = false;
			}
			break;

		case GLFW_KEY_Z:
			if (action == GLFW_PRESS) {
				controller->is_z_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_z_key_pressed = false;
			}
			break;

		case GLFW_KEY_C:
			if (action == GLFW_PRESS) {
				controller->is_c_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_c_key_pressed = false;
			}
			break;
		// endregion
		
		// region RF
		case GLFW_KEY_R:
			if (action == GLFW_PRESS) {
				controller->is_r_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_r_key_pressed = false;
			}
			break;

		case GLFW_KEY_F:
			if (action == GLFW_PRESS) {
				controller->is_f_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				controller->is_f_key_pressed = false;
			}
			break;
		// endregion
	}
}

void space_box_controller_update(SpaceBoxController *controller) {
	btTransform *bt_transform = btRigidBody_getWorldTransform(controller->rigid_body);
	
	vec3 force;
	glm_vec3_dup(FORWARD, force);
	vec3 torque;

	versor rigid_body_rotation;
	btTransform_getRotation(bt_transform, rigid_body_rotation);
	
	if (controller->is_end_key_pressed) {
		vec3 zero_vector;
		glm_vec3_zero(zero_vector);
		btRigidBody_setLinearVelocity(controller->rigid_body, zero_vector);
		btRigidBody_setAngularVelocity(controller->rigid_body, zero_vector);
	}
	
	// region WASD
	if (controller->is_w_key_pressed) {
		glm_quat_rotatev(rigid_body_rotation, force, force);
		btRigidBody_applyCentralForce(controller->rigid_body, force);
	}

	if (controller->is_s_key_pressed) {
		glm_quat_rotatev(rigid_body_rotation, force, force);
		glm_vec3_negate(force);
		btRigidBody_applyCentralForce(controller->rigid_body, force);
	}
	// endregion
	
	// region QEZC
	if (controller->is_z_key_pressed) {
		glm_quat_rotatev(rigid_body_rotation, UP, torque);
		btRigidBody_applyTorque(controller->rigid_body, torque);
	}
	
	if (controller->is_c_key_pressed) {
		glm_quat_rotatev(rigid_body_rotation, UP, torque);
		glm_vec3_negate(torque);
		btRigidBody_applyTorque(controller->rigid_body, torque);
	}
	// endregion
	
	// region RF
	if (controller->is_f_key_pressed) {
		glm_quat_rotatev(rigid_body_rotation, RIGHT, torque);
		btRigidBody_applyTorque(controller->rigid_body, torque);
	}
	
	if (controller->is_r_key_pressed) {
		glm_quat_rotatev(rigid_body_rotation, RIGHT, torque);
		glm_vec3_negate(torque);
		btRigidBody_applyTorque(controller->rigid_body, torque);
	}
	// endregion
}

void space_box_controller_delete(SpaceBoxController *controller) {
	free(controller);
}
