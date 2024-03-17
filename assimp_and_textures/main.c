#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "logger.h"
#include <cglm/cglm.h>
#include "opengl_error_detector.h"
#include "vertex.h"
#include "mesh.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <libbmp/libbmp.h>

static void error_callback(int error_code, const char* description) {
    error("Error: %d; %s\n", error_code, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static Mesh *triangle_mesh;
static Mesh *box_mesh;

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

#define NUMBER_OF_VERTICES_IN_A_FACE 3

static const struct aiScene *scene = NULL;

static void init_geometry() {
	triangle_mesh = mesh_new(3, 3);
	
	// Vertex 1
	triangle_mesh->vertices[0].position[0] = -0.6;
	triangle_mesh->vertices[0].position[1] = -0.4;
	triangle_mesh->vertices[0].position[2] = 0.0;
	
	triangle_mesh->vertices[0].normal[0] = 0.0;
	triangle_mesh->vertices[0].normal[1] = 0.0;
	triangle_mesh->vertices[0].normal[2] = 1.0;
	
	triangle_mesh->vertices[0].color[0] = 1.0;
	triangle_mesh->vertices[0].color[1] = 0.0;
	triangle_mesh->vertices[0].color[2] = 0.0;
	
	// Vertex 2
	triangle_mesh->vertices[1].position[0] = 0.6;
	triangle_mesh->vertices[1].position[1] = -0.4;
	triangle_mesh->vertices[1].position[2] = 0.0;
	
	triangle_mesh->vertices[1].normal[0] = 0.0;
	triangle_mesh->vertices[1].normal[1] = 0.0;
	triangle_mesh->vertices[1].normal[2] = 1.0;
	
	triangle_mesh->vertices[1].color[0] = 0.0;
	triangle_mesh->vertices[1].color[1] = 1.0;
	triangle_mesh->vertices[1].color[2] = 0.0;
	
	// Vertex 3
	triangle_mesh->vertices[2].position[0] = 0.0;
	triangle_mesh->vertices[2].position[1] = 0.6;
	triangle_mesh->vertices[2].position[2] = 0.0;
	
	triangle_mesh->vertices[2].normal[0] = 0.0;
	triangle_mesh->vertices[2].normal[1] = 0.0;
	triangle_mesh->vertices[2].normal[2] = 1.0;
	
	triangle_mesh->vertices[2].color[0] = 0.0;
	triangle_mesh->vertices[2].color[1] = 0.0;
	triangle_mesh->vertices[2].color[2] = 1.0;
	
	triangle_mesh->indices[0] = 0;
	triangle_mesh->indices[1] = 1;
	triangle_mesh->indices[2] = 2;
	
	
	
	scene = aiImportFile("./meshes/box.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene) {
		error("Error loading box.obj\n");
	}
	if (scene->mNumMeshes == 0) {
		error("Nothing loaded from box.obj\n");
	}
	struct aiMesh *ai_box_mesh = scene->mMeshes[0];
	box_mesh = mesh_new(ai_box_mesh->mNumVertices, ai_box_mesh->mNumFaces * NUMBER_OF_VERTICES_IN_A_FACE);
	for (int i = 0; i < ai_box_mesh->mNumVertices; i++) {
		struct aiVector3D *aiVertex = &ai_box_mesh->mVertices[i];
		struct aiVector3D *aiNormal = &ai_box_mesh->mNormals[i];
		
		Vertex *vertex = &box_mesh->vertices[i];
		vec3 *position = &vertex->position;
		vec3 *normal = &vertex->normal;
		vec3 *color = &vertex->color;
		
		position[0][0] = aiVertex->x;
		position[0][1] = aiVertex->y;
		position[0][2] = aiVertex->z;
		
		normal[0][0] = aiNormal->x;
		normal[0][1] = aiNormal->y;
		normal[0][2] = aiNormal->z;
		
		color[0][0] = 1;
		color[0][1] = 1;
		color[0][2] = 1;
	}
	
	for (int i = 0; i < ai_box_mesh->mNumFaces; i++) {
		struct aiFace *ai_face = &ai_box_mesh->mFaces[i];
		
		int base_index = i * NUMBER_OF_VERTICES_IN_A_FACE;
		
		box_mesh->indices[base_index] = ai_face->mIndices[0];
		box_mesh->indices[base_index + 1] = ai_face->mIndices[1];
		box_mesh->indices[base_index + 2] = ai_face->mIndices[2];
	}
}

static void init_bitmaps() {
	bmp_img wood_box_wall;
	bmp_img_read(&wood_box_wall, "./bitmaps/wood_box_wall.bmp");
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

	return vertex_array;
}

static void render_mesh(GLuint program, GLuint vao, int framebuffer_width, int framebuffer_height, Mesh *mesh) {
	glUseProgram(program);
	glBindVertexArray(vao);
	
    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint model_matrix_location = glGetUniformLocation(program, "model_matrix");
    const GLint directional_light_color_location = glGetUniformLocation(program, "directional_light.color");
    const GLint directional_light_direction_location = glGetUniformLocation(program, "directional_light.direction");
	
	const float ratio = framebuffer_width / (float) framebuffer_height;
	
	mat4 p, mvp;
	mat4 m = GLM_MAT4_IDENTITY_INIT;
	glm_translate_z(m, -2);
	glm_rotate_y(m, glfwGetTime(), m);
	glm_perspective(glm_rad(90), ratio, 0.1, 1000, p);
	glm_mat4_mul(p, m, mvp);
	
	vec3 directional_light_direction = { 0, 0, -1 };
	glm_vec3_normalize(directional_light_direction);

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp[0]);
	glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, m[0]);
	glUniform3f(directional_light_color_location, 1, 1, 1);
	glUniform3fv(directional_light_direction_location, 1, (const GLfloat *)  &directional_light_direction);
	
	glDrawElements(GL_TRIANGLES, mesh->number_of_indices, GL_UNSIGNED_SHORT, NULL);
	check_opengl_errors("rendering");
}

int main(int argc, char **argv) {
	opengl_error_detector_init();
	init_geometry();
	init_bitmaps();

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
        bitmapInfo.width,
        bitmapInfo.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        bitmapInfo.data.data()
    );

    m_textures[name] = TextureInfo{ texture, bitmapInfo.width, bitmapInfo.height };

    glGenerateMipmap(GL_TEXTURE_2D);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
		int width, height;
        glfwGetFramebufferSize(window, &width, &height);
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		glBindTexture(GL_TEXTURE_2D, 0);
		render_mesh(program, vertex_array, width, height, mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
