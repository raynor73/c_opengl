#include "renderer.h"
#include "opengl_error_detector.h"
#include <GLFW/glfw3.h>
#include "constants.h"
#include "game_object.h"

void render_mesh(
	GLuint program,
	Transform *camera_transform,
	mat4 projection_matrix,
	GameObject *game_object
) {
	glUseProgram(program);
	glBindVertexArray(game_object->vao);
	
    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint model_matrix_location = glGetUniformLocation(program, "model_matrix");
    const GLint directional_light_color_location = glGetUniformLocation(program, "directional_light.color");
    const GLint directional_light_direction_location = glGetUniformLocation(program, "directional_light.direction");
    const GLint texture_uniform = glGetUniformLocation(program, "texture_uniform");
    const GLint texture_scale_uniform = glGetUniformLocation(program, "texture_scale_uniform");
		
	mat4 mvp;
	mat4 view_matrix;
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	mat4 model_view_matrix;
	
	vec3 look_at_coordinate;
	glm_quat_rotatev(camera_transform->rotation, FORWARD, look_at_coordinate);
	glm_vec3_add(look_at_coordinate, camera_transform->position, look_at_coordinate);
	vec3 rotated_up;
	glm_quat_rotatev(camera_transform->rotation, UP, rotated_up);
	glm_lookat(camera_transform->position, look_at_coordinate, rotated_up, view_matrix);
	
	glm_translate(m, game_object->transform.position);
	glm_quat_rotate(m, game_object->transform.rotation, m);
	glm_scale(m, game_object->transform.scale);
		
	glm_mat4_mul(view_matrix, m, model_view_matrix);
	glm_mat4_mul(projection_matrix, model_view_matrix, mvp);
	
	vec3 directional_light_direction = { 0, -1, -1 };
	glm_vec3_normalize(directional_light_direction);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp[0]);
	glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, m[0]);
	glUniform3f(directional_light_color_location, 1, 1, 1);
	glUniform3fv(directional_light_direction_location, 1, (const GLfloat *)  &directional_light_direction);
	
	glBindTexture(GL_TEXTURE_2D, game_object->material.texture);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texture_uniform, 0);
	glUniform2fv(texture_scale_uniform, 1, game_object->material.texture_scale);

	glDrawElements(GL_TRIANGLES, game_object->mesh->number_of_indices, GL_UNSIGNED_SHORT, NULL);
	check_opengl_errors("rendering");
}

void render_unlit_mesh(
	GLuint program,
	Transform *camera_transform,
	mat4 projection_matrix,
	GameObject *game_object
) {
	glUseProgram(program);
	glBindVertexArray(game_object->vao);
	
    const GLint mvp_location = glGetUniformLocation(program, "mvp_uniform");
    const GLint texture_uniform = glGetUniformLocation(program, "texture_uniform");
    const GLint texture_scale_uniform = glGetUniformLocation(program, "texture_scale_uniform");
		
	mat4 mvp;
	mat4 view_matrix;
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	mat4 model_view_matrix;
	
	vec3 look_at_coordinate;
	glm_quat_rotatev(camera_transform->rotation, FORWARD, look_at_coordinate);
	glm_vec3_add(look_at_coordinate, camera_transform->position, look_at_coordinate);
	vec3 rotated_up;
	glm_quat_rotatev(camera_transform->rotation, UP, rotated_up);
	glm_lookat(camera_transform->position, look_at_coordinate, rotated_up, view_matrix);
	
	glm_translate(m, game_object->transform.position);
	glm_quat_rotate(m, game_object->transform.rotation, m);
	glm_scale(m, game_object->transform.scale);
		
	glm_mat4_mul(view_matrix, m, model_view_matrix);
	glm_mat4_mul(projection_matrix, model_view_matrix, mvp);
	
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp[0]);
	
	glBindTexture(GL_TEXTURE_2D, game_object->material.texture);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texture_uniform, 0);
	glUniform2fv(texture_scale_uniform, 1, game_object->material.texture_scale);

	glDrawElements(GL_TRIANGLES, game_object->mesh->number_of_indices, GL_UNSIGNED_SHORT, NULL);
	check_opengl_errors("unlit rendering");
}
