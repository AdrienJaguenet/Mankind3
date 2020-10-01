#include "mesh.h"

mesh_t mesh_new(vec3_t * vertices, vec3_t * normals, vec2_t * uvs,
				GLuint * indices)
{
	mesh_t mesh;

	mesh.vertices = vertices;
	mesh.normals = normals;
	mesh.uvs = uvs;
	mesh.indices = indices;

	INFO("Got here!");

	/* Generate the VAO. */
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	/* Generate the buffer objects.. */
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh.vertices) * sizeof(vec3_t),
				 &mesh.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.nbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh.normals) * sizeof(vec3_t),
				 &mesh.normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh.uvs) * sizeof(vec2_t),
				 &mesh.uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh.indices) * sizeof(GLuint),
				 &mesh.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.uvbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

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
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->nbo);
	glDeleteBuffers(1, &mesh->uvbo);
	glDeleteBuffers(1, &mesh->ebo);

	glDeleteVertexArrays(1, &mesh->vao);
}
