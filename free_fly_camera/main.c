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
#include "material.h"
#include "constants.h"

static Transform camera_transform;
 
static float camera_y_angle = 0;
static float camera_x_angle = 0;

static bool is_w_key_pressed = false;
static bool is_s_key_pressed = false;
static bool is_a_key_pressed = false;
static bool is_d_key_pressed = false;

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static bool is_prev_cursor_position_available = false;
static double prev_pointer_position_x = 0;
static double prev_pointer_position_y = 0;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (is_prev_cursor_position_available) {
		camera_y_angle -= GLM_PI_4 *((xpos - prev_pointer_position_x) / (1399.0 / 2));
		camera_x_angle -= GLM_PI_4 *((ypos - prev_pointer_position_y) / (1399.0 / 2));
		
		versor camera_y_rotation;
		versor camera_x_rotation;
		glm_quat(camera_y_rotation, camera_y_angle, 0, 1, 0);
		glm_quat(camera_x_rotation, camera_x_angle, 1, 0, 0);
		glm_quat_mul(camera_y_rotation, camera_x_rotation, camera_transform.rotation);
	} 
	prev_pointer_position_x = xpos;
	prev_pointer_position_y = ypos;
	is_prev_cursor_position_available = true;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			break;
			
		case GLFW_KEY_W:
			if (action == GLFW_PRESS) {
				is_w_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				is_w_key_pressed = false;
			}
			break;

		case GLFW_KEY_S:
			if (action == GLFW_PRESS) {
				is_s_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				is_s_key_pressed = false;
			}
			break;

		case GLFW_KEY_A:
			if (action == GLFW_PRESS) {
				is_a_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				is_a_key_pressed = false;
			}
			break;

		case GLFW_KEY_D:
			if (action == GLFW_PRESS) {
				is_d_key_pressed = true;
			} else if (action == GLFW_RELEASE) {
				is_d_key_pressed = false;
			}
			break;
	}
}

// TODO Add following args: camera
static void render_mesh(
	GLuint program, 
	GLuint vao, 
	int framebuffer_width, 
	int framebuffer_height, 
	Mesh *mesh,
	Material *material
) {
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
	
	glBindTexture(GL_TEXTURE_2D, material->texture);
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
	Material box_material;

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
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	box_material.texture = wooden_box_wall_texture;
    
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
 
		vec3 movement;
		if (is_w_key_pressed) {
			glm_quat_rotatev(camera_transform.rotation, FORWARD, movement);
			glm_vec3_scale(movement, dt, movement);
			glm_vec3_add(camera_transform.position, movement, camera_transform.position);
		}
		if (is_s_key_pressed) {
			glm_quat_rotatev(camera_transform.rotation, FORWARD, movement);
			glm_vec3_scale(movement, dt, movement);
			glm_vec3_sub(camera_transform.position, movement, camera_transform.position);
		}
		if (is_a_key_pressed) {
			glm_quat_rotatev(camera_transform.rotation, RIGHT, movement);
			glm_vec3_scale(movement, dt, movement);
			glm_vec3_sub(camera_transform.position, movement, camera_transform.position);
		}
		if (is_d_key_pressed) {
			glm_quat_rotatev(camera_transform.rotation, RIGHT, movement);
			glm_vec3_scale(movement, dt, movement);
			glm_vec3_add(camera_transform.position, movement, camera_transform.position);
		}
 
		render_mesh(program, vertex_array, width, height, mesh, &box_material);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
