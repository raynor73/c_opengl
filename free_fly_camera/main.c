#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "logger.h"
#include <cglm/cglm.h>
#include "opengl_error_detector.h"
#include "vertex.h"
#include "mesh.h"
#include <libbmp/libbmp.h>
#include "mesh_loader.h"

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"uniform mat4 model_matrix;\n"
"in vec2 uv;\n"
"in vec3 vPos;\n"
"in vec3 normal;\n"
"out vec3 normal_varying;\n"
"out vec2 uv_varying;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"	 normal_varying = (model_matrix * vec4(normal, 0.0)).xyz;\n"
"	 uv_varying = uv;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"struct DirectionalLight {\n"
"    vec3 color;\n"
"    vec3 direction;\n"
"};\n"
"uniform DirectionalLight directional_light;\n"
"in vec3 normal_varying;\n"
"in vec2 uv_varying;\n"
"uniform sampler2D texture_uniform;"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"	fragment =\n"
"            texture2D(texture_uniform, uv_varying) * vec4(directional_light.color, 1.0) *\n"
"            dot(normalize(normal_varying), -directional_light.direction);\n"
"}\n";

#define TEXTURE_BYTES_PER_PIXEL 4
static GLuint create_texture_from_file(const char *path) {
	bmp_img bitmap;
	bmp_img_read(&bitmap, path);
	uint8_t *bitmap_data = (uint8_t *) malloc(bitmap.img_header.biWidth * bitmap.img_header.biHeight * TEXTURE_BYTES_PER_PIXEL);
	uint32_t bitmap_data_offset = 0;
	for (int y = 0; y < bitmap.img_header.biHeight; y++) {
		for (int x = 0; x < bitmap.img_header.biWidth; x++) {
			bmp_pixel pixel;
			if (x == 0 && y == 0) {
				pixel.red = 255;
				pixel.green = 0;
				pixel.blue = 0;
			} else {
				pixel = bitmap.img_pixels[y][x];
			}
				
			bitmap_data[bitmap_data_offset++] = pixel.red;
			bitmap_data[bitmap_data_offset++] = pixel.green;
			bitmap_data[bitmap_data_offset++] = pixel.blue;
			bitmap_data[bitmap_data_offset++] = 255;
		}
	}
	
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        bitmap.img_header.biWidth,
        bitmap.img_header.biHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        bitmap_data
    );
    
	glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
	
	check_opengl_errors("load texture");
	
	bmp_img_free(&bitmap);
	free(bitmap_data);
	
	return texture;
}

static GLuint compile_and_link_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
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

static GLuint setup_vao_for_mesh(GLuint program, const Mesh *mesh) {
	GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->number_of_vertices, mesh->vertices, GL_STATIC_DRAW);
    
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * mesh->number_of_indices, mesh->indices, GL_STATIC_DRAW);
     
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint normal_location = glGetAttribLocation(program, "normal");
    const GLint uv_location = glGetAttribLocation(program, "uv");
    check_opengl_errors("getting uniform and attribute locations");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(normal_location);
    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glEnableVertexAttribArray(uv_location);
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, uv));
    check_opengl_errors("vertex attributes initialization");

	return vertex_array;
}

static void render_mesh(GLuint program, GLuint vao, int framebuffer_width, int framebuffer_height, Mesh *mesh) {
	glUseProgram(program);
	glBindVertexArray(vao);
	
    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint model_matrix_location = glGetUniformLocation(program, "model_matrix");
    const GLint directional_light_color_location = glGetUniformLocation(program, "directional_light.color");
    const GLint directional_light_direction_location = glGetUniformLocation(program, "directional_light.direction");
    const GLint texture_uniform = glGetUniformLocation(program, "texture_uniform");
	
	const float ratio = framebuffer_width / (float) framebuffer_height;
	
	mat4 p, mvp;
	
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
	mat4 model_view_matrix;
	
	glm_rotate_y(view_matrix, 1, view_matrix);
	glm_translate_z(view_matrix, -2);
	
	glm_translate_z(m, -2);
	glm_rotate_y(m, glfwGetTime(), m);
	
	glm_perspective(glm_rad(90), ratio, 0.1, 1000, p);
	
	glm_mat4_mul(view_matrix, m, model_view_matrix);
	glm_mat4_mul(p, model_view_matrix, mvp);
	
	vec3 directional_light_direction = { 0, 0, -1 };
	glm_vec3_normalize(directional_light_direction);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp[0]);
	glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, m[0]);
	glUniform3f(directional_light_color_location, 1, 1, 1);
	glUniform3fv(directional_light_direction_location, 1, (const GLfloat *)  &directional_light_direction);
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texture_uniform, 0);

	glDrawElements(GL_TRIANGLES, mesh->number_of_indices, GL_UNSIGNED_SHORT, NULL);
	check_opengl_errors("rendering");
}

int main(int argc, char **argv) {
	opengl_error_detector_init();
	Mesh *box_mesh = load_mesh("./meshes/box.obj");

	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
    	error("glfwInit() failed\n");
    }

	glfwSetErrorCallback(error_callback);

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "Assimp and Textures", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        error("glfwCreateWindow() failed");
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	gladLoadGL((GLADloadfunc) glfwGetProcAddress);
	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(1);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    GLuint program = compile_and_link_shader_program(vertex_shader_text, fragment_shader_text);
    Mesh *mesh = box_mesh;
    GLuint vertex_array = setup_vao_for_mesh(program, mesh);
	GLuint wooden_box_wall_texture = create_texture_from_file("./bitmaps/wood_box_wall.bmp");
    
   
   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
		int width, height;
        glfwGetFramebufferSize(window, &width, &height);
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		glBindTexture(GL_TEXTURE_2D, wooden_box_wall_texture);
		render_mesh(program, vertex_array, width, height, mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
