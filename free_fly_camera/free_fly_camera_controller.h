#ifndef ILAPIN_FREE_FLY_CAMERA_CONTROLLER_H
#define ILAPIN_FREE_FLY_CAMERA_CONTROLLER_H

#include "transform.h"

typedef struct FreeFlyCameraController FreeFlyCameraController;

FreeFlyCameraController *free_fly_camera_controller_new(Transform *camera_transform);
void free_fly_camera_controller_on_cursor_position_updated(FreeFlyCameraController *controller, float position_x, float poition_y);
void free_fly_camera_controller_on_key_event(FreeFlyCameraController *controller, int key, int scancode, int action, int mods);
void free_fly_camera_controller_update(float dt);
void free_fly_camera_controller_delete(FreeFlyCameraController *controller);

#endif
