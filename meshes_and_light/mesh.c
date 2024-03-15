#include "mesh.h"
#include <stdlib.h>
#include <string.h>
#include "logger.h"

Mesh *mesh_new(uint32_t number_of_vertices, uint32_t number_of_indices) {
	Mesh *mesh = (Mesh *) malloc(sizeof(Mesh));
	if (mesh == NULL) {
		error("Failed to allocate memory for mesh\n");
	}
	
	size_t size_of_vertices_buffer = sizeof(Vertex) * mesh->number_of_vertices;
	size_t size_of_indices_buffer = sizeof(uint16_t) * mesh->number_of_indices;
	
	mesh->number_of_vertices = number_of_vertices;
	mesh->number_of_indices = number_of_indices;
	
	mesh->vertices = (Vertex *) malloc(size_of_vertices_buffer);
	if (mesh->vertices == NULL) {
		error("Failed to allocate memory for mesh verices\n");
	}
	
	mesh->indices = (uint16_t *) malloc(size_of_indices_buffer);
	if (mesh->indices == NULL) {
		error("Failed to allocate memory for mesh indices\n");
	}
	
	memset(mesh->vertices, 0, size_of_vertices_buffer);
	memset(mesh->indices, 0, size_of_indices_buffer);
	
	return mesh;
}

void mesh_delete(Mesh *mesh) {
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh);
}

