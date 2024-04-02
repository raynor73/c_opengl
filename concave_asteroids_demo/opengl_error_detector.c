#include "opengl_error_detector.h"
#include "logger.h"
#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>

#define INFO_BUFFER_SIZE 1024

static GLchar info_buffer[INFO_BUFFER_SIZE];
static GHashTable *opengl_error_map;
static GHashTable *framebuffer_status_map;
static GLenum opengl_error_codes[] = {
	GL_INVALID_ENUM,
	GL_INVALID_VALUE,
	GL_INVALID_OPERATION,
	GL_STACK_OVERFLOW,
	GL_STACK_UNDERFLOW,
	GL_OUT_OF_MEMORY,
	GL_INVALID_FRAMEBUFFER_OPERATION
};
static const char *opengl_errors[] = {
	"GL_INVALID_ENUM",
	"GL_INVALID_VALUE",
	"GL_INVALID_OPERATION",
	"GL_STACK_OVERFLOW",
	"GL_STACK_UNDERFLOW",
	"GL_OUT_OF_MEMORY",
	"GL_INVALID_FRAMEBUFFER_OPERATION"
};
static GLenum framebuffer_status_codes[] = {
	GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
	GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
	GL_FRAMEBUFFER_UNSUPPORTED
};
static const char *framebuffer_statuses[] = {
	"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT",
	"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
	"GL_FRAMEBUFFER_UNSUPPORTED"
};

void opengl_error_detector_init() {
	opengl_error_map = g_hash_table_new(g_int_hash, g_int_equal);
	framebuffer_status_map = g_hash_table_new(g_int_hash, g_int_equal);

	for (int i = 0; i < sizeof(opengl_error_codes) / sizeof(GLenum); i++) {
		g_hash_table_insert(opengl_error_map, &opengl_error_codes[i], (gpointer) opengl_errors[i]);
	}

	for (int i = 0; i < sizeof(framebuffer_status_codes) / sizeof(GLenum); i++) {
		g_hash_table_insert(framebuffer_status_map, &framebuffer_status_codes[i], (gpointer) framebuffer_statuses[i]);
	}
}

void check_opengl_errors(const char *location_name) {
	GLenum error_code = glGetError();
	bool is_error_detected = false;
    while (error_code != GL_NO_ERROR) {
		is_error_detected = true;
		if (g_hash_table_contains(opengl_error_map, &error_code)) {
			const char *error_text = g_hash_table_lookup(opengl_error_map, &error_code);
			log_error("OpenGL error detected at %s: %s\n", location_name, error_text);
		} else {
			log_error("OpenGL error detected at %s: unknown error %d\n", location_name, error_code);
		}

        error_code = glGetError();
    }

	if (is_error_detected) {
		error("OpenGL error detected, exiting.\n");
	}
}

void check_shader_compilation_error(GLuint shader, const char *location_name) {
	GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        glGetShaderInfoLog(shader, INFO_BUFFER_SIZE, NULL, info_buffer);
        error("OpenGL shader compilation failure detected at %s: %s\n", location_name, info_buffer);
    }
}

void check_shader_linking_error(GLuint shader, const char *location_name) {
	GLint link_status;
    glGetProgramiv(shader, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        log_error("OpenGL shader linking failure detected at %s\n", location_name);

        GLint max_length = 0;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        GLchar *info_log = (GLchar *) malloc(max_length * sizeof(GLchar));
        glGetProgramInfoLog(shader, max_length, NULL, info_log);
        error("%s\n", info_log);
    }
}

void check_framebuffer_status(const char *location_name) {
	GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        if (g_hash_table_contains(framebuffer_status_map, &framebuffer_status)) {
            const char *status_description = g_hash_table_lookup(framebuffer_status_map, &framebuffer_status);
            error("Incomplete framebuffer status at %s: %s\n", location_name, status_description);
        } else {
			error("Incomplete framebuffer status at %s: Unknown status %d\n", location_name, framebuffer_status);
        }
    }
}
