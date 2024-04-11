#include "mesh_loader.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "logger.h"

#define NUMBER_OF_VERTICES_IN_A_FACE 3

Mesh *load_mesh(const char *path) {
	const struct aiScene *scene = aiImportFile(path, 0);
	if (!scene) {
		error("Error loading %s\n", path);
	}
	if (scene->mNumMeshes == 0) {
		error("Nothing loaded from %s\n", path);
	}
	struct aiMesh *ai_box_mesh = scene->mMeshes[0];
	Mesh *mesh = mesh_new(ai_box_mesh->mNumVertices, ai_box_mesh->mNumFaces * NUMBER_OF_VERTICES_IN_A_FACE);
	for (int i = 0; i < ai_box_mesh->mNumVertices; i++) {
		struct aiVector3D *aiVertex = &ai_box_mesh->mVertices[i];
		struct aiVector3D *aiNormal = &ai_box_mesh->mNormals[i];
		struct aiVector3D *ai_uv = &ai_box_mesh->mTextureCoords[0][i];
		
		Vertex *vertex = &mesh->vertices[i];
		vec3 *position = &vertex->position;
		vec3 *normal = &vertex->normal;
		vec3 *color = &vertex->color;
		vec2 *uv = &vertex->uv;
		
		position[0][0] = aiVertex->x;
		position[0][1] = aiVertex->y;
		position[0][2] = aiVertex->z;
		
		normal[0][0] = aiNormal->x;
		normal[0][1] = aiNormal->y;
		normal[0][2] = aiNormal->z;
		
		color[0][0] = 1;
		color[0][1] = 1;
		color[0][2] = 1;
		
		uv[0][0] = ai_uv->x;
		uv[0][1] = ai_uv->y;
	}
	
	for (int i = 0; i < ai_box_mesh->mNumFaces; i++) {
		struct aiFace *ai_face = &ai_box_mesh->mFaces[i];
		
		int base_index = i * NUMBER_OF_VERTICES_IN_A_FACE;
		
		mesh->indices[base_index] = ai_face->mIndices[0];
		mesh->indices[base_index + 1] = ai_face->mIndices[1];
		mesh->indices[base_index + 2] = ai_face->mIndices[2];
	}
	
	aiReleaseImport(scene);
	
	return mesh;
}
