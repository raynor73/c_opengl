#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "logger.h"
#include <cglm/cglm.h>
#include "opengl_error_detector.h"
#include "vertex.h"
#include "mesh.h"

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static Mesh *mesh;

/*#define NUMBER_OF_VERTICES 3
static Vertex vertices[NUMBER_OF_VERTICES] = {
	{ { -0.6, -0.4, 0.0 }, { 0.0, 0.0, -1.0 }, { 1.0, 0.0, 0.0 } },
    { {  0.6, -0.4, 0.0 }, { 0.0, 0.0, -1.0 }, { 0.f, 1.f, 0.f } },
    { {  0.0,  0.6, 0.0 }, { 0.0, 0.0, -1.0 }, { 0.f, 0.f, 1.f } }	
};
#define NUMBER_OF_INDICES 3
static uint16_t indices[NUMBER_OF_INDICES] = { 0, 1, 2 };*/

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"uniform mat4 model_matrix;\n"
"in vec3 vCol;\n"
"in vec3 vPos;\n"
"in vec3 normal;\n"
"out vec3 color;\n"
"out vec3 normal_varying;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"	 normal_varying = (model_matrix * vec4(normal, 0.0)).xyz;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"struct DirectionalLight {\n"
"    vec3 color;\n"
"    vec3 direction;\n"
"};\n"
"uniform DirectionalLight directional_light;\n"
"in vec3 normal_varying;\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"	fragment =\n"
"            vec4(color, 1.0) * vec4(directional_light.color, 1.0) *\n"
"            dot(normalize(normal_varying), -directional_light.direction);\n"
"}\n";

static void init_geometry() {
	mesh = mesh_new(3, 3);
	
	// Vertex 1
	mesh->vertices[0].position[0] = -0.6;
	mesh->vertices[0].position[1] = -0.4;
	mesh->vertices[0].position[2] = 0.0;
	
	mesh->vertices[0].normal[0] = 0.0;
	mesh->vertices[0].normal[1] = 0.0;
	mesh->vertices[0].normal[2] = -1.0;
	
	mesh->vertices[0].color[0] = 1.0;
	mesh->vertices[0].color[1] = 0.0;
	mesh->vertices[0].color[2] = 0.0;
	
	// Vertex 2
	mesh->vertices[1].position[0] = 0.6;
	mesh->vertices[1].position[1] = -0.4;
	mesh->vertices[1].position[2] = 0.0;
	
	mesh->vertices[1].normal[0] = 0.0;
	mesh->vertices[1].normal[1] = 0.0;
	mesh->vertices[1].normal[2] = -1.0;
	
	mesh->vertices[1].color[0] = 0.0;
	mesh->vertices[1].color[1] = 1.0;
	mesh->vertices[1].color[2] = 0.0;
	
	// Vertex 3
	mesh->vertices[2].position[0] = 0.0;
	mesh->vertices[2].position[1] = 0.6;
	mesh->vertices[2].position[2] = 0.0;
	
	mesh->vertices[2].normal[0] = 0.0;
	mesh->vertices[2].normal[1] = 0.0;
	mesh->vertices[2].normal[2] = -1.0;
	
	mesh->vertices[2].color[0] = 0.0;
	mesh->vertices[2].color[1] = 0.0;
	mesh->vertices[2].color[2] = 1.0;
	
	mesh->indices[0] = 0;
	mesh->indices[1] = 1;
	mesh->indices[2] = 2;
}

int main(int argc, char **argv) {
	opengl_error_detector_init();
	init_geometry();

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
    window = glfwCreateWindow(640, 480, "Meshes and Lights", NULL, NULL);
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
     
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
	check_shader_compilation_error(vertex_shader, "vertex shader");
 
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
	check_shader_compilation_error(fragment_shader, "fragment shader");
 
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    check_shader_linking_error(program, "shader program");
 
    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint model_matrix_location = glGetUniformLocation(program, "model_matrix");
    const GLint directional_light_color_location = glGetUniformLocation(program, "directional_light.color");
    const GLint directional_light_direction_location = glGetUniformLocation(program, "directional_light.direction");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");
    const GLint normal_location = glGetAttribLocation(program, "normal");
    check_opengl_errors("getting uniform and attribute locations");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, color));
    glEnableVertexAttribArray(normal_location);
    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, normal));
    check_opengl_errors("vertex attributes initialization");


	vec3 directional_light_direction = { -1, 0, 1 };
	glm_vec3_normalize(directional_light_direction);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
		int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        mat4 p, mvp;
        mat4 m = GLM_MAT4_IDENTITY_INIT;
        glm_translate_z(m, -2);
        glm_rotate_y(m, glfwGetTime(), m);
        glm_perspective(glm_rad(90), ratio, 0.1, 1000, p);
        glm_mat4_mul(p, m, mvp);
		
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp[0]);
        
        glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, m[0]);
        glUniform3f(directional_light_color_location, 1, 1, 1);
        glUniform3fv(directional_light_direction_location, 1, (const GLfloat *)  &directional_light_direction);
        
        glDrawElements(GL_TRIANGLES, mesh->number_of_indices, GL_UNSIGNED_SHORT, NULL);
        check_opengl_errors("rendering");
		

      
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
