#ifndef ILAPIN_MESH_H
#define ILAPIN_MESH_H

#include "vertex.h"
#include <stdint.h>

typedef struct Mesh {
	Vertex *vertices;
	uint16_t *indices;
	uint32_t number_of_vertices;
	uint32_t number_of_indices;
} Mesh;

Mesh *mesh_new(uint32_t number_of_vertices, uint32_t number_of_indices);
void mesh_delete(Mesh *mesh);

#endif
