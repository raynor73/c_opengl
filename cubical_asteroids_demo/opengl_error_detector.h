#ifndef ILAPIN_OPENGL_ERROR_DETECTOR_H
#define ILAPIN_OPENGL_ERROR_DETECTOR_H

#include <glad/gl.h>

void opengl_error_detector_init();
void check_opengl_errors(const char *location_name);
void check_shader_compilation_error(GLuint shader, const char *location_name);
void check_shader_linking_error(GLuint shader, const char *location_name);
void check_framebuffer_status(const char *location_name);

#endif
