#ifndef ILAPIN_CAMERA_H
#define ILAPIN_CAMERA_H

#include "transform.h"
#include <cglm/cglm.h>

typedef struct Camera {
	Transform transform;
	void (*get_projection_matrix)(mat4 projection_matrix);
} Camera;

#endif
