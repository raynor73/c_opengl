#include "game_object.h"
#include "logger.h"
#include <stdlib.h>

GameObject *game_object_new(
	Material *material,
	Mesh *mesh,
	GLuint vao,
	Transform transform
) {
	GameObject *game_object = (GameObject *) malloc(sizeof(GameObject));
	if (!game_object) {
		error("Failed to allocate memory for Game Object\n");
	}
	
	game_object->material = material;
	game_object->mesh = mesh;
	game_object->vao = vao;
	game_object->transform = transform;
	
	return game_object;
}
void game_object_delete(GameObject *game_object) {
	free(game_object);
}
