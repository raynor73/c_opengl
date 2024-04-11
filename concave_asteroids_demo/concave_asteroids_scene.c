#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
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
#include "concave_asteroids_scene.h"
#include "text_renderer.h"
#include "fps_counter.h"

static TextRenderer *text_renderer;

static SpaceBoxController *controller;

static DynamicsWorld *dynamics_world;

static PerspectiveCamera camera;

static GLuint program;
static GLuint unlit_shader_program;

static Texture wooden_box_wall_texture;
static Texture sky_texture;
static Texture asteroid_texture;

static Material sky_material;
static Material wood_material;
static Material asteroid_material;

static GArray *physics_to_graphics_transform_links;

static GameObject sky_sphere;
static GameObject box;

#define NUMBER_OF_ASTEROIDS_IN_A_ROW 10
static const float distance_between_asteroids = 1;
static const float asteroid_mass = 1;

static GameObject *asteroids[NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW];
static BoxRigidBody *asteroid_rigid_bodies[NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW];

static GameObject convex_asteroid;

#define FPS_STRING_BUFFER_SIZE 32
static char fps_string_buffer[FPS_STRING_BUFFER_SIZE];

static RingBuffer fps_counter_ring_buffer;

void concave_asterodis_scene_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			break;
	}
	
	space_box_controller_on_key_event(controller, key, scancode, action, mods);
}

void concave_asterodis_scene_start(void) {
	ring_buffer_init(&fps_counter_ring_buffer, 100 * sizeof(float));
	
	// region Init physics world
	dynamics_world = init_dynamic_world();	
	vec3 gravity = { 0, 0, 0 };
	btDiscreteDynamicsWorld_setGravity(dynamics_world->dynamics_world, gravity);
	// endregion
	
	// region Init rigid bodies
	for (int k = 0; k < NUMBER_OF_ASTEROIDS_IN_A_ROW; k++) {
		for (int j = 0; j < NUMBER_OF_ASTEROIDS_IN_A_ROW; j++) {
			for (int i = 0; i < NUMBER_OF_ASTEROIDS_IN_A_ROW; i++) {
				vec3 asteroid_box_half_extents = { 0.5, 0.5, 0.5 };
				vec3 asteroid_box_origin = { 
					i * (2 * asteroid_box_half_extents[0] + distance_between_asteroids) - (2 * asteroid_box_half_extents[0] + distance_between_asteroids) * NUMBER_OF_ASTEROIDS_IN_A_ROW / 2, 
					j * (2 * asteroid_box_half_extents[1] + distance_between_asteroids) - (2 * asteroid_box_half_extents[1] + distance_between_asteroids) * NUMBER_OF_ASTEROIDS_IN_A_ROW / 2, 
					-k * (2 * asteroid_box_half_extents[2] + distance_between_asteroids)
				};
				int rigid_body_index = i + j * NUMBER_OF_ASTEROIDS_IN_A_ROW + k * NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW;
				asteroid_rigid_bodies[rigid_body_index] = create_box_rigid_body(asteroid_mass, asteroid_box_half_extents, asteroid_box_origin);
				btCollisionObject_setActivationState(asteroid_rigid_bodies[rigid_body_index]->rigid_body, DISABLE_DEACTIVATION);
				btDiscreteDynamicsWorld_addRigidBody(dynamics_world->dynamics_world, asteroid_rigid_bodies[rigid_body_index]->rigid_body);
			}
		}
	}
	
	vec3 falling_box_half_extents = { 0.5, 0.5, 0.5 };
	vec3 falling_box_origin = { 0, 0, 2 };
	BoxRigidBody *falling_box_rigid_body = create_box_rigid_body(1, falling_box_half_extents, falling_box_origin);
	btDiscreteDynamicsWorld_addRigidBody(dynamics_world->dynamics_world, falling_box_rigid_body->rigid_body);
	btCollisionObject_setActivationState(falling_box_rigid_body->rigid_body, DISABLE_DEACTIVATION);
	// endregion
	
	controller = space_box_controller_new(falling_box_rigid_body->rigid_body);
	
	// region Cameras init
	camera.fov = 90;
	camera.near = 0.1;
	camera.far = 1000;
	glm_vec3_zero(camera.transform.position);
	glm_quat_identity(camera.transform.rotation);
	// endregion
	
	// region Shaders init
	char *vertex_shader_text = load_text_file("./shaders/vertex_shader.glsl");
	char *fragment_shader_text = load_text_file("./shaders/fragment_shader.glsl");
    program = compile_and_link_shader_program(vertex_shader_text, fragment_shader_text);
    free(vertex_shader_text);
    free(fragment_shader_text);
    
	char *unlit_vertex_shader_text = load_text_file("./shaders/unlit_vertex_shader.glsl");
	char *unlit_fragment_shader_text = load_text_file("./shaders/unlit_fragment_shader.glsl");
    unlit_shader_program = compile_and_link_shader_program(unlit_vertex_shader_text, unlit_fragment_shader_text);
    free(unlit_vertex_shader_text);
    free(unlit_fragment_shader_text);
    // endregion
    
    // region Meshes init
    Mesh *box_mesh = load_mesh("./meshes/box.obj");
    GLuint box_vao = setup_vao_for_mesh(program, box_mesh);
    
    Mesh *sky_sphere_mesh = load_mesh("./meshes/sky_sphere.obj");
    GLuint sky_sphere_vao = setup_unlit_shader_vao_for_mesh(unlit_shader_program, sky_sphere_mesh);
    
    Mesh *vertical_plane_mesh = load_mesh("./meshes/vertical_plane.obj");
    GLuint vertical_plane_vao = setup_unlit_shader_vao_for_mesh(unlit_shader_program, vertical_plane_mesh);
    
    Mesh *convex_asteroid_mesh = load_mesh("./meshes/convex_asteroid.obj");
    GLuint convex_asteroid_vao = setup_vao_for_mesh(program, convex_asteroid_mesh);
    // endregion
    
    text_renderer = text_renderer_new(unlit_shader_program, vertical_plane_mesh, vertical_plane_vao);
    
    // region Textures init
	wooden_box_wall_texture = create_texture_from_file("./bitmaps/wood_box_wall.bmp");
	sky_texture = create_texture_from_file("./bitmaps/2k_stars_milky_way.bmp");
	asteroid_texture = create_texture_from_file("./bitmaps/ground_0010_color_1k.bmp");
	// endregion
	
	// region Materials init
	sky_material.texture = sky_texture;
	glm_vec2_one(sky_material.texture_scale);
	
	wood_material.texture = wooden_box_wall_texture;
	glm_vec2_one(wood_material.texture_scale);
	
	asteroid_material.texture = asteroid_texture;
	glm_vec2_one(asteroid_material.texture_scale);
	// endregion
	
	physics_to_graphics_transform_links = g_array_new(FALSE, FALSE, sizeof(PhysicsToGraphicsTransformLink));
	
	// region Game Objects init
	sky_sphere.vao = sky_sphere_vao;
	sky_sphere.mesh = sky_sphere_mesh;
	sky_sphere.material = &sky_material;
	glm_vec3_zero(sky_sphere.transform.position);
	glm_quat_identity(sky_sphere.transform.rotation);
	sky_sphere.transform.scale[0] = 10;
	sky_sphere.transform.scale[1] = 10;
	sky_sphere.transform.scale[2] = 10;
	
	box.vao = box_vao;
	box.mesh = box_mesh;
	box.material = &wood_material;
	glm_vec3_zero(box.transform.position);
	box.transform.position[2] = -2;
	glm_quat_identity(box.transform.rotation);
	glm_vec3_one(box.transform.scale);
	
	convex_asteroid.vao = convex_asteroid_vao;
	convex_asteroid.mesh = convex_asteroid_mesh;
	convex_asteroid.material = &asteroid_material;
	convex_asteroid.transform.position[0] = 0;
	convex_asteroid.transform.position[1] = 0;
	convex_asteroid.transform.position[2] = 3;
	glm_quat_identity(convex_asteroid.transform.rotation);
	glm_vec3_one(convex_asteroid.transform.scale);
	    
	for (int k = 0; k < NUMBER_OF_ASTEROIDS_IN_A_ROW; k++) {
		for (int j = 0; j < NUMBER_OF_ASTEROIDS_IN_A_ROW; j++) {
			for (int i = 0; i < NUMBER_OF_ASTEROIDS_IN_A_ROW; i++) {
				Transform transform;
				glm_vec3_one(transform.scale);
				PhysicsToGraphicsTransformLink link;
				int asteroid_index = i + j * NUMBER_OF_ASTEROIDS_IN_A_ROW + k * NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW;
				asteroids[asteroid_index] = game_object_new(&asteroid_material, box_mesh, box_vao, transform);
				
				link.bt_transform = btRigidBody_getWorldTransform(asteroid_rigid_bodies[asteroid_index]->rigid_body);
				link.transform = &asteroids[asteroid_index]->transform;
				g_array_append_vals(physics_to_graphics_transform_links, &link, 1);
			}
		}
	}
	// endregion
	
	// region Physics linking
    PhysicsToGraphicsTransformLink link;
    link.bt_transform = btRigidBody_getWorldTransform(falling_box_rigid_body->rigid_body);
    link.transform = &box.transform;
    g_array_append_vals(physics_to_graphics_transform_links, &link, 1);
    // endregion
}

void concave_asteroids_scene_update(GLFWwindow *window, float dt) {
	space_box_controller_update(controller);
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
	glm_quat_copy(camera.transform.rotation, origin_transform.rotation);
	render_unlit_mesh(unlit_shader_program, &origin_transform, projection_matrix, &sky_sphere);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	render_mesh(program, &camera.transform, projection_matrix, &box);
	
	render_mesh(program, &camera.transform, projection_matrix, &convex_asteroid);
	
	for (int i = 0; i < NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW * NUMBER_OF_ASTEROIDS_IN_A_ROW; i++) {
		render_mesh(program, &camera.transform, projection_matrix, asteroids[i]);
	}
	
	// region FPS
	glClear(GL_DEPTH_BUFFER_BIT);
	
	float fps = calculate_fps(&fps_counter_ring_buffer, dt);
	memset(fps_string_buffer, 0, FPS_STRING_BUFFER_SIZE);
	sprintf(fps_string_buffer, "FPS: %.1f", fps);
	
	vec2 text_position;
	text_position[0] = 10;
	text_position[1] = 10;
	text_renderer_draw_text_line(text_renderer, width, height, text_position, fps_string_buffer);
	// endregion
}
