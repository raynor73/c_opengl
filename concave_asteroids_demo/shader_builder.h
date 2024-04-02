#ifndef ILAPIN_SHADER_BUILDER_H
#define ILAPIN_SHADER_BUILDER_H

#include <glad/gl.h>

GLuint compile_and_link_shader_program(const char *vertex_shader_source, const char *fragment_shader_source);

#endif
