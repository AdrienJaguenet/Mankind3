#include "mesh.h"

mesh_t mesh_new(vec3_t * vertices, vec3_t * indices)
{
	mesh_t mesh;

	mesh.vertices = vertices;
	mesh.indices = indices;

	INFO("Got here!");

	/* Generate the VAO. */
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	/* Generate the VBO. */
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh.vertices) * sizeof(vec3_t),
				 &mesh.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh.indices) * sizeof(GLuint),
				 &mesh.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	return mesh;
}

void mesh_render(mesh_t * mesh)
{
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	glDrawElements(GL_TRIANGLES, sizeof(mesh->indices), GL_UNSIGNED_INT,
				   (void *) 0);

	glBindVertexArray(0);
}

void mesh_terminate(mesh_t * mesh)
{
	// Delete the buffer objects.
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);

	// Delete the vertex array.
	glDeleteVertexArrays(1, &mesh->vao);
}
