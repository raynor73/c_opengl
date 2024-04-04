#ifndef ILAPIN_TEXT_RENDERER_H
#define ILAPIN_TEXT_RENDERER_H

#include "game_object.h"
#include "camera.h"
#include "texture_factory.h"
#include <glad/gl.h>

#define NUMBER_OF_CHARACTERS 128

typedef struct TextRenderer {
	Texture glyph_textures[NUMBER_OF_CHARACTERS];
	vec2 advances[NUMBER_OF_CHARACTERS];
	GameObject glyph;
	OrthoCamera ortho_camera;
	GLuint unlit_shader_program;
} TextRenderer;

TextRenderer *text_renderer_new(GLuint unlit_shader_program, Mesh *vertical_plane_mesh, GLuint vertical_plane_vao);
void text_renderer_draw_text_line(
	TextRenderer *renderer,
	int viewport_width,
	int viewport_height,
	vec2 position,
	char *text
);

#endif
