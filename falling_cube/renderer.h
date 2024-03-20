#ifndef ILAPIN_RENDERER_H
#define ILAPIN_RENDERER_H

#include "transform.h"
#include "mesh.h"
#include "material.h"
#include <glad/gl.h>
#include <cglm/cglm.h>

void render_mesh(
	GLuint program,
	GLuint vao,
	Transform *camera_transform,
	mat4 projection_matrix,
	Mesh *mesh,
	Material *material
);

#endif
