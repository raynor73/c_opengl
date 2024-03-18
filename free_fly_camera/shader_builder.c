#include "shader_builder.h"
#include <stddef.h>
#include "opengl_error_detector.h"

GLuint compile_and_link_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
	check_shader_compilation_error(vertex_shader, "vertex shader");
 
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
	check_shader_compilation_error(fragment_shader, "fragment shader");
 
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    check_shader_linking_error(program, "shader program");
    
    return program;
} 

