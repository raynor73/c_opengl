#include "text_renderer.h"
#include "renderer.h"
#include "logger.h"
#include <cglm/cglm.h>

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
	renderer->glyph.transform.position[0] = 0;
	renderer->glyph.transform.position[1] = 0;
	renderer->glyph.transform.position[2] = -1;
	glm_quat_identity(renderer->glyph.transform.rotation);
	glm_vec3_one(renderer->glyph.transform.scale);
	
		if (FT_Init_FreeType(&ft_library)) {
		error("Failed to init FreeType library\n");
	}
	if (FT_New_Face(ft_library, "./fonts/Roboto-Regular.ttf", 0, &ft_typeface)) {
		error("Failed to init Roboto Regular typeface\n");
	}
	if (FT_Set_Pixel_Sizes(ft_typeface, 0, 16)) {
		error("Failed to set pixel size for typeface\n");
	}
	if (FT_Load_Char(ft_typeface, 'P', FT_LOAD_RENDER)) {
		error("Failed to load character\n");
	}
	if (FT_Render_Glyph(ft_typeface->glyph, FT_RENDER_MODE_NORMAL)) {
		error("Failed to render glyph\n");
	}
	
	// region Face init
	//log_debug("glyph width: %d; rows: %d\n", ft_typeface->glyph->bitmap.width, ft_typeface->glyph->bitmap.rows);
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
	glyph_material.texture = create_texture_from_memory(
		ft_typeface->glyph->bitmap.width,
		ft_typeface->glyph->bitmap.rows,
		glyph_data
	);
	free(glyph_data);
    // endregion
	
	return renderer;
}

void text_renderer_draw_text_line(TextRenderer *renderer, int viewport_width, int viewport_height, char *text) {
}
