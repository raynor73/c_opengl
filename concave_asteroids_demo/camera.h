#ifndef ILAPIN_CAMERA_H
#define ILAPIN_CAMERA_H

#include "transform.h"
#include <cglm/cglm.h>

typedef struct PerspectiveCamera {
	float fov;
	float near;
	float far;
	Transform transform;
} PerspectiveCamera;

typedef struct OrthoCamera {
	float top;
	float right;
	float bottom;
	float left;
	float near;
	float far;
	Transform transform;
} OrthoCamera;

#endif
