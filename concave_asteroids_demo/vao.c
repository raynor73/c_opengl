#include "vao.h"
#include "opengl_error_detector.h"

GLuint setup_vao_for_mesh(GLuint program, const Mesh *mesh) {
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

GLuint setup_unlit_shader_vao_for_mesh(GLuint program, const Mesh *mesh) {
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
     
    const GLint vpos_location = glGetAttribLocation(program, "position_attribute");
    const GLint uv_location = glGetAttribLocation(program, "uv_attribute");
    check_opengl_errors("getting uniform and attribute locations");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(uv_location);
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, uv));
    check_opengl_errors("vertex attributes initialization");

	return vertex_array;
}
