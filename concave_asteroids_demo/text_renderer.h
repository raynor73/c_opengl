#ifndef ILAPIN_TEXT_RENDERER_H
#define ILAPIN_TEXT_RENDERER_H

#include "game_object.h"
#include "camera.h"
#include <glad/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct TextRenderer {
	GameObject glyph;
	OrthoCamera ortho_camera;
	GLuint unlit_shader_program;
	FT_Library ft_library;
	FT_Face ft_typeface;
} TextRenderer;

TextRenderer *text_renderer_new(GLuint unlit_shader_program, Mesh *vertical_plane_mesh, GLuint vertical_plane_vao);
void text_renderer_draw_text_line(TextRenderer *renderer, int viewport_width, int viewport_height, char *text);

#endif
