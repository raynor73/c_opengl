#include "text_renderer.h"
#include "renderer.h"
#include "logger.h"
#include <string.h>
#include <cglm/cglm.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static Material glyph_material;

TextRenderer *text_renderer_new(GLuint unlit_shader_program, Mesh *vertical_plane_mesh, GLuint vertical_plane_vao) {
	TextRenderer *renderer = (TextRenderer *) malloc(sizeof(TextRenderer));
	if (!renderer) {
		error("Failed to allocate memory for Text Renderer\n");
	}
	
	renderer->unlit_shader_program = unlit_shader_program;
	
	renderer->ortho_camera.near = 0.1;
	renderer->ortho_camera.far = 1000;
	glm_vec3_zero(renderer->ortho_camera.transform.position);
	glm_quat_identity(renderer->ortho_camera.transform.rotation);
	
	renderer->glyph.vao = vertical_plane_vao;
	renderer->glyph.mesh = vertical_plane_mesh;
	renderer->glyph.material = &glyph_material;
	glm_vec2_one(renderer->glyph.material->texture_scale);
	renderer->glyph.transform.position[0] = 0;
	renderer->glyph.transform.position[1] = 0;
	renderer->glyph.transform.position[2] = -1;
	glm_quat_identity(renderer->glyph.transform.rotation);
	glm_vec3_one(renderer->glyph.transform.scale);
	
	FT_Library ft_library;
	FT_Face ft_typeface;	
	if (FT_Init_FreeType(&ft_library)) {
		error("Failed to init FreeType library\n");
	}
	if (FT_New_Face(ft_library, "./fonts/Roboto-Regular.ttf", 0, &ft_typeface)) {
		error("Failed to init Roboto Regular typeface\n");
	}
	if (FT_Set_Pixel_Sizes(ft_typeface, 0, 16)) {
		error("Failed to set pixel size for typeface\n");
	}
	
	// region Face init
	//log_debug("glyph width: %d; rows: %d\n", ft_typeface->glyph->bitmap.width, ft_typeface->glyph->bitmap.rows);
	for (int i = 32; i < NUMBER_OF_CHARACTERS; i++) {
		if (FT_Load_Char(ft_typeface, i, FT_LOAD_RENDER)) {
			error("Failed to load character\n");
		}
		if (FT_Render_Glyph(ft_typeface->glyph, FT_RENDER_MODE_NORMAL)) {
			error("Failed to render glyph\n");
		}
		uint8_t *glyph_data = (uint8_t *) malloc(ft_typeface->glyph->bitmap.width * ft_typeface->glyph->bitmap.rows * TEXTURE_BYTES_PER_PIXEL);
		if (!glyph_data) {
			error("Failed to allocate memory for glyph bitmap\n");
		}
		int index = 0;
		for (int y = ft_typeface->glyph->bitmap.rows - 1; y >= 0; y--) {
			for (int x = 0; x < ft_typeface->glyph->bitmap.width; x++) {
				uint8_t value = ft_typeface->glyph->bitmap.buffer[index++];
				glyph_data[(x + y * ft_typeface->glyph->bitmap.width) * TEXTURE_BYTES_PER_PIXEL] = value;
				glyph_data[(x + y * ft_typeface->glyph->bitmap.width) * TEXTURE_BYTES_PER_PIXEL + 1] = value;
				glyph_data[(x + y * ft_typeface->glyph->bitmap.width) * TEXTURE_BYTES_PER_PIXEL + 2] = value;
				glyph_data[(x + y * ft_typeface->glyph->bitmap.width) * TEXTURE_BYTES_PER_PIXEL + 3] = value;
			}
		}
		renderer->glyph_textures[i] = create_texture_from_memory(
			ft_typeface->glyph->bitmap.width,
			ft_typeface->glyph->bitmap.rows,
			glyph_data
		);
		renderer->advances[i][0] = ft_typeface->glyph->advance.x;
		renderer->advances[i][1] = ft_typeface->glyph->advance.y;
		free(glyph_data);
	}
	
	FT_Done_Face(ft_typeface);
	FT_Done_FreeType(ft_library);
    // endregion
	
	return renderer;
}

void text_renderer_draw_text_line(
	TextRenderer *renderer,
	int viewport_width,
	int viewport_height,
	vec2 position,
	char *text
) {
	int position_x = position[0];
	mat4 projection_matrix;
	glm_ortho(0, viewport_width - 1, 0, viewport_height - 1, renderer->ortho_camera.near, renderer->ortho_camera.far, projection_matrix);
	for (int i = 0; i < strlen(text); i++) {
		int character = text[i];
		renderer->glyph.transform.scale[0] = renderer->glyph_textures[character].width;
		renderer->glyph.transform.scale[1] = renderer->glyph_textures[character].height;
		renderer->glyph.transform.position[0] = position_x + renderer->glyph_textures[character].width / 2;
		renderer->glyph.transform.position[1] = position[1] + renderer->glyph_textures[character].height / 2;
		renderer->glyph.material->texture = renderer->glyph_textures[character];
		render_unlit_mesh(renderer->unlit_shader_program, &renderer->ortho_camera.transform, projection_matrix, &renderer->glyph);
		position_x += renderer->advances[character][0] / 64;
	}
}
