#ifndef ILAPIN_TEXTURE_FACTORY_H
#define ILAPIN_TEXTURE_FACTORY_H

#include <glad/gl.h>

#define TEXTURE_BYTES_PER_PIXEL 4

typedef struct Texture {
	GLsizei width;
	GLsizei height;
	GLuint handle;
} Texture;

Texture create_texture_from_file(const char *path);
Texture create_texture_from_memory(uint32_t width, uint32_t height, uint8_t *data);

#endif
