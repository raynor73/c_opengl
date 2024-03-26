#ifndef ILAPIN_TRANSFORM_H
#define ILAPIN_TRANSFORM_H

#include <cglm/cglm.h>

typedef struct Transform {
	vec3 position;
	versor rotation;
	vec3 scale;
} Transform;

#endif
