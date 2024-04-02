#include "free_fly_camera_controller.h"
#include <stdbool.h>
#include <stdlib.h>
#include "logger.h"
#include <string.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "constants.h"

typedef struct FreeFlyCameraController {
	float camera_y_angle;
	float camera_x_angle;

	bool is_w_key_pressed;
	bool is_s_key_pressed;
	bool is_a_key_pressed;
	bool is_d_key_pressed;
	
	bool is_prev_cursor_position_available;
	double prev_pointer_position_x;
	double prev_pointer_position_y;
	
	Transform *camera_transform;
} FreeFlyCameraController;

FreeFlyCameraController *free_fly_camera_controller_new(Transform *camera_transform) {
	FreeFlyCameraController *controller = (FreeFlyCameraController *) malloc(sizeof(FreeFlyCameraController));
	if (controller == NULL) {
		error("Failed to allocated memeory for Free Fly Camera controller\n");
	}
	
	memset(controller, 0, sizeof(FreeFlyCameraController));
	
	controller->camera_transform = camera_transform;
	
	return controller;
}

void free_fly_camera_controller_delete(FreeFlyCameraController *controller) {
	free(controller);
}

void free_fly_camera_controller_on_cursor_position_updated(FreeFlyCameraController *controller, float position_x, float position_y) {
	if (controller->is_prev_cursor_position_available) {
		controller->camera_y_angle -= GLM_PI_4 *((position_x - controller->prev_pointer_position_x) / (1399.0 / 2));
		controller->camera_x_angle -= GLM_PI_4 *((position_y - controller->prev_pointer_position_y) / (1399.0 / 2));
		
		versor camera_y_rotation;
		versor camera_x_rotation;
		glm_quat(camera_y_rotation, controller->camera_y_angle, 0, 1, 0);
		glm_quat(camera_x_rotation, controller->camera_x_angle, 1, 0, 0);
		glm_quat_mul(camera_y_rotation, camera_x_rotation, controller->camera_transform->rotation);
	} 
	controller->prev_pointer_position_x = position_x;
	controller->prev_pointer_position_y = position_y;
	controller->is_prev_cursor_position_available = true;
}

void free_fly_camera_controller_on_key_event(FreeFlyCameraController *controller, int key, int scancode, int action, int mods) {
	switch (key) {
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
	}
}

void free_fly_camera_controller_update(FreeFlyCameraController *controller, float dt) {
	vec3 movement;
	float velocity = 10;
	if (controller->is_w_key_pressed) {
		glm_quat_rotatev(controller->camera_transform->rotation, FORWARD, movement);
		glm_vec3_scale(movement, velocity * dt, movement);
		glm_vec3_add(controller->camera_transform->position, movement, controller->camera_transform->position);
	}
	if (controller->is_s_key_pressed) {
		glm_quat_rotatev(controller->camera_transform->rotation, FORWARD, movement);
		glm_vec3_scale(movement, velocity * dt, movement);
		glm_vec3_sub(controller->camera_transform->position, movement, controller->camera_transform->position);
	}
	if (controller->is_a_key_pressed) {
		glm_quat_rotatev(controller->camera_transform->rotation, RIGHT, movement);
		glm_vec3_scale(movement, velocity * dt, movement);
		glm_vec3_sub(controller->camera_transform->position, movement, controller->camera_transform->position);
	}
	if (controller->is_d_key_pressed) {
		glm_quat_rotatev(controller->camera_transform->rotation, RIGHT, movement);
		glm_vec3_scale(movement, velocity * dt, movement);
		glm_vec3_add(controller->camera_transform->position, movement, controller->camera_transform->position);
	}
}
