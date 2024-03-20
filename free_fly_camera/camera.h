#ifndef ILAPIN_CAMERA_H
#define ILAPIN_CAMERA_H

#include "transform.h"
#include <cglm/cglm.h>

typedef struct PerspectiveCamera {
	float fov;
	float aspect_ratio;
	float near;
	float far;
	Transform transform;
} PerspectiveCamera;

#endif
