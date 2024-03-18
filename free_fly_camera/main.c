#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "logger.h"
#include <cglm/cglm.h>
#include "opengl_error_detector.h"
#include "vertex.h"
#include "mesh.h"
#include "mesh_loader.h"
#include "fs.h"
#include "texture_factory.h"
#include "vao.h"
#include "shader_builder.h"
#include "transform.h"

static vec3 UP = { 0, 1, 0 };
static vec3 FORWARD = { 0, 0, -1 };
static Transform camera_transform; 

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static bool is_w_key_pressed = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			is_w_key_pressed = true;
		} else if (action == GLFW_RELEASE) {
			is_w_key_pressed = false;
		}
	}
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
	mat4 view_matrix;
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	mat4 model_view_matrix;
	
	vec3 look_at_coordinate;
	glm_quat_rotatev(camera_transform.rotation, FORWARD, look_at_coordinate);
	glm_vec3_add(look_at_coordinate, camera_transform.position, look_at_coordinate);
	glm_lookat(camera_transform.position, look_at_coordinate, UP, view_matrix);
	
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
	
	camera_transform.position[0] = 0;
	camera_transform.position[1] = 0;
	camera_transform.position[2] = 2;
	glm_quat_identity(camera_transform.rotation);
		
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

	const char *vertex_shader_text = load_text_file("./shaders/vertex_shader.glsl");
	const char *fragment_shader_text = load_text_file("./shaders/fragment_shader.glsl");
    GLuint program = compile_and_link_shader_program(vertex_shader_text, fragment_shader_text);
    Mesh *mesh = box_mesh;
    GLuint vertex_array = setup_vao_for_mesh(program, mesh);
	GLuint wooden_box_wall_texture = create_texture_from_file("./bitmaps/wood_box_wall.bmp");
    
    /* Loop until the user closes the window */
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
		double current_time = glfwGetTime();
		float dt = current_time - prev_time;
        prev_time = current_time;
		
		int width, height;
        glfwGetFramebufferSize(window, &width, &height);
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		if (is_w_key_pressed) {
			camera_transform.position[2] -= dt;
		}
 
		glBindTexture(GL_TEXTURE_2D, wooden_box_wall_texture);
		render_mesh(program, vertex_array, width, height, mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
