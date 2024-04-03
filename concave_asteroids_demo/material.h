#ifndef ILAPIN_MATERIAL_H
#define ILAPIN_MATERIAL_H

#include <glad/gl.h>
#include <cglm/cglm.h>
#include "texture_factory.h"

typedef struct Material {
	Texture texture;
	vec2 texture_scale;
} Material;

#endif
