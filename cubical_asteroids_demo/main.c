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
#include "renderer.h"
#include "game_object.h"
#include "bullet_physics.h"
#include "physics.h"
#include <glib.h>
#include <string.h>
#include "space_box_controller.h"

static SpaceBoxController *controller;

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			break;
	}
	
	space_box_controller_on_key_event(controller, key, scancode, action, mods);
}

int main(int argc, char **argv) {
	DynamicsWorld *dynamics_world = init_dynamic_world();	
	vec3 gravity = { 0, 0, 0 };
	btDiscreteDynamicsWorld_setGravity(dynamics_world->dynamics_world, gravity);
	
	vec3 ground_shape_half_extents = { 5, 0.5, 5 };
	vec3 origin = { 0, -2.5, 0 };
	BoxRigidBody *ground_rigid_body = create_box_rigid_body(0, ground_shape_half_extents, origin);
	btDiscreteDynamicsWorld_addRigidBody(dynamics_world->dynamics_world, ground_rigid_body->rigid_body);

	vec3 falling_box_half_extents = { 0.5, 0.5, 0.5 };
	vec3 falling_box_origin = { 0, 0, -2 };
	BoxRigidBody *falling_box_rigid_body = create_box_rigid_body(1, falling_box_half_extents, falling_box_origin);
	btRigidBody_setSleepingThresholds(falling_box_rigid_body, 0, 0);
	btDiscreteDynamicsWorld_addRigidBody(dynamics_world->dynamics_world, falling_box_rigid_body->rigid_body);

	controller = space_box_controller_new(falling_box_rigid_body->rigid_body);

	opengl_error_detector_init();
	
	PerspectiveCamera camera;
	camera.fov = 90;
	camera.near = 0.1;
	camera.far = 1000;
	camera.transform.position[0] = 0;
	camera.transform.position[1] = 0;
	camera.transform.position[2] = 2;
	glm_quat_identity(camera.transform.rotation);
	
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
    window = glfwCreateWindow(800, 600, "Falling Cube", NULL, NULL);
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

	// region Shaders init
	char *vertex_shader_text = load_text_file("./shaders/vertex_shader.glsl");
	char *fragment_shader_text = load_text_file("./shaders/fragment_shader.glsl");
    GLuint program = compile_and_link_shader_program(vertex_shader_text, fragment_shader_text);
    free(vertex_shader_text);
    free(fragment_shader_text);
    
	char *unlit_vertex_shader_text = load_text_file("./shaders/unlit_vertex_shader.glsl");
	char *unlit_fragment_shader_text = load_text_file("./shaders/unlit_fragment_shader.glsl");
    GLuint unlit_shader_program = compile_and_link_shader_program(unlit_vertex_shader_text, unlit_fragment_shader_text);
    free(unlit_vertex_shader_text);
    free(unlit_fragment_shader_text);
    // endregion
    
    // region Meshes init
    Mesh *box_mesh = load_mesh("./meshes/box.obj");
    GLuint box_vao = setup_vao_for_mesh(program, box_mesh);
    
    Mesh *sky_sphere_mesh = load_mesh("./meshes/sky_sphere.obj");
    GLuint sky_sphere_vao = setup_unlit_shader_vao_for_mesh(unlit_shader_program, sky_sphere_mesh);
    // endregion
    
    // region Testures init
	GLuint wooden_box_wall_texture = create_texture_from_file("./bitmaps/wood_box_wall.bmp");
	GLuint concrete_squares_texture = create_texture_from_file("./bitmaps/concrete_squares.bmp");
	GLuint sky_texture = create_texture_from_file("./bitmaps/2k_stars_milky_way.bmp");
	// endregion
	
	// region Game Objects init
	GameObject sky_sphere;
	sky_sphere.vao = sky_sphere_vao;
	sky_sphere.mesh = sky_sphere_mesh;
	sky_sphere.material.texture = sky_texture;
	glm_vec2_one(sky_sphere.material.texture_scale);
	glm_vec3_zero(sky_sphere.transform.position);
	glm_quat_identity(sky_sphere.transform.rotation);
	sky_sphere.transform.scale[0] = 10;
	sky_sphere.transform.scale[1] = 10;
	sky_sphere.transform.scale[2] = 10;
	
	GameObject box;
	box.vao = box_vao;
	box.mesh = box_mesh;
	box.material.texture = wooden_box_wall_texture;
	glm_vec2_one(box.material.texture_scale);
	glm_vec3_zero(box.transform.position);
	box.transform.position[2] = -2;
	glm_quat_identity(box.transform.rotation);
	glm_vec3_one(box.transform.scale);
    
	GameObject ground;
	ground.vao = box_vao;
	ground.mesh = box_mesh;
	ground.material.texture = concrete_squares_texture;
	ground.material.texture_scale[0] = 10;
	ground.material.texture_scale[1] = 10;
	glm_vec3_zero(ground.transform.position);
	ground.transform.position[1] = -2;
	glm_quat_identity(ground.transform.rotation);
	ground.transform.scale[0] = 10;
	ground.transform.scale[1] = 1;
	ground.transform.scale[2] = 10;
	// endregion
    
    // region Physics linking
    GArray *physics_to_graphics_transform_links = g_array_new(FALSE, FALSE, sizeof(PhysicsToGraphicsTransformLink));
    PhysicsToGraphicsTransformLink link;
    link.bt_transform = btRigidBody_getWorldTransform(falling_box_rigid_body->rigid_body);
    link.transform = &box.transform;
    g_array_append_vals(physics_to_graphics_transform_links, &link, 1);
    link.bt_transform = btRigidBody_getWorldTransform(ground_rigid_body->rigid_body);
    link.transform = &ground.transform;
    g_array_append_vals(physics_to_graphics_transform_links, &link, 1);
    // endregion
    
    /* Loop until the user closes the window */
    double prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
		double current_time = glfwGetTime();
		float dt = current_time - prev_time;
        prev_time = current_time;

		space_box_controller_update(controller);
		btRigidBody_activate(falling_box_rigid_body->rigid_body);
		btDiscreteDynamicsWorld_stepSimulation(dynamics_world->dynamics_world, dt, 10);
		sync_physics_and_graphics_transforms(physics_to_graphics_transform_links);
		
		int width, height;
        glfwGetFramebufferSize(window, &width, &height);
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		vec3 camera_position = { 0, 1, 1.5 };
		glm_quat_rotatev(box.transform.rotation, camera_position, camera_position);
		glm_vec3_add(box.transform.position, camera_position, camera.transform.position);
		glm_quat_copy(box.transform.rotation, camera.transform.rotation);
		mat4 projection_matrix;
		glm_perspective(glm_rad(camera.fov), width / (float) height, camera.near, camera.far, projection_matrix);
		
		Transform origin_transform;
		glm_vec3_zero(origin_transform.position);
		memcpy(origin_transform.rotation, camera.transform.rotation, sizeof(versor));
		render_unlit_mesh(unlit_shader_program, &origin_transform, projection_matrix, &sky_sphere);
		
        glClear(GL_DEPTH_BUFFER_BIT);
		
		render_mesh(program, &camera.transform, projection_matrix, &box);
		render_mesh(program, &camera.transform, projection_matrix, &ground);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
