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
#include "camera.h"
#include "free_fly_camera_controller.h"
#include "renderer.h"
#include "game_object.h"

static FreeFlyCameraController *free_fly_camera_controller;

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	free_fly_camera_controller_on_cursor_position_updated(free_fly_camera_controller, xpos, ypos);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			break;
	}
	
	free_fly_camera_controller_on_key_event(free_fly_camera_controller, key, scancode, action, mods);
}

int main(int argc, char **argv) {
	opengl_error_detector_init();
	
	PerspectiveCamera camera;
	camera.fov = 90;
	camera.near = 0.1;
	camera.far = 1000;
	camera.transform.position[0] = 0;
	camera.transform.position[1] = 0;
	camera.transform.position[2] = 2;
	glm_quat_identity(camera.transform.rotation);
	
	free_fly_camera_controller = free_fly_camera_controller_new(&camera.transform);
	
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
    
    Mesh *box_mesh = load_mesh("./meshes/box.obj");
    GLuint box_vao = setup_vao_for_mesh(program, box_mesh);
	GLuint wooden_box_wall_texture = create_texture_from_file("./bitmaps/wood_box_wall.bmp");
	//GLuint concrete_squares_texture = create_texture_from_file("./bitmaps/concrete_squares.bmp");
	
	GameObject box;
	box.vao = box_vao;
	box.mesh = box_mesh;
	box.material.texture = wooden_box_wall_texture;
	glm_vec3_zero(box.transform.position);
	box.transform.position[2] = -2;
	glm_quat_identity(box.transform.rotation);
	glm_vec3_one(box.transform.scale);
    
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
 
		free_fly_camera_controller_update(free_fly_camera_controller, dt);
		mat4 projection_matrix;
		glm_perspective(glm_rad(camera.fov), width / (float) height, camera.near, camera.far, projection_matrix);
		
		glm_quat(box.transform.rotation, glfwGetTime(), 0, 1, 0);

		render_mesh(program, box.vao, &camera.transform, projection_matrix, &box);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
