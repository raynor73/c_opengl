#ifndef ILAPIN_MATERIAL_H
#define ILAPIN_MATERIAL_H

#include <glad/gl.h>
#include <cglm/cglm.h>

typedef struct Material {
	GLuint texture;
	vec2 texture_scale;
} Material;

#endif
