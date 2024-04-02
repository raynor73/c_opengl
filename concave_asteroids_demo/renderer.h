#ifndef ILAPIN_RENDERER_H
#define ILAPIN_RENDERER_H

#include "transform.h"
#include <glad/gl.h>
#include <cglm/cglm.h>
#include "game_object.h"

void render_mesh(
	GLuint program,
	Transform *camera_transform,
	mat4 projection_matrix,
	GameObject *game_object
);

void render_unlit_mesh(
	GLuint program,
	Transform *camera_transform,
	mat4 projection_matrix,
	GameObject *game_object
);

#endif
