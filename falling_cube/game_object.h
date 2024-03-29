#ifndef ILAPIN_GAME_OBJECT_H
#define ILAPIN_GAME_OBJECT_H

#include <glad/gl.h>
#include "transform.h"
#include "mesh.h"
#include "material.h"

typedef struct GameObject {
	Material material;
	Mesh *mesh;
	GLuint vao;
	Transform transform;
} GameObject;

/*GameObject *game_object_new(
	const char *mesh_path,
	const char *
	GLuint texture
);
void game_object_delete(GameObject *game_object);*/

#endif
