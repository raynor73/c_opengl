#ifndef ILAPIN_VAO_H
#define ILAPIN_VAO_H

#include <glad/gl.h>
#include "mesh.h"

GLuint setup_vao_for_mesh(GLuint program, const Mesh *mesh);
GLuint setup_unlit_shader_vao_for_mesh(GLuint program, const Mesh *mesh);

#endif
