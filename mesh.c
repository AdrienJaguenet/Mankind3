#include "mesh.h"

#include <stdlib.h>
#include <string.h>

mesh_t mesh_new(vec3_t * vertices, vec3_t * normals, vec2_t * uvs,
				GLuint * indices)
{
	mesh_t mesh;

	mesh.vertices = vertices;
	mesh.normals = normals;
	mesh.uvs = uvs;
	mesh.indices = indices;

	INFO("Got here!");
	mesh_load(&mesh);
	return mesh;
}

void mesh_load(mesh_t * mesh)
{
	/* Generate the VAO. */
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	/* Generate the buffer objects.. */
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices_no * sizeof(vec3_t),
				 mesh->vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->nbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices_no * sizeof(vec3_t),
				 mesh->normals, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->uvbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices_no * sizeof(vec2_t),
				 mesh->uvs, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->vertices_no * sizeof(GLuint),
				 mesh->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->uvbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	INFO("Loaded mesh with %u vertices", mesh->vertices_no);
}

void mesh_render(mesh_t * mesh)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture->index);

	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	glDrawElements(GL_TRIANGLES, mesh->vertices_no, GL_UNSIGNED_INT,
				   (void *) 0);

	glBindVertexArray(0);
}

void mesh_terminate(mesh_t * mesh)
{
	free(mesh->vertices);
	free(mesh->normals);
	free(mesh->uvs);
	free(mesh->indices);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->nbo);
	glDeleteBuffers(1, &mesh->uvbo);
	glDeleteBuffers(1, &mesh->ebo);

	glDeleteVertexArrays(1, &mesh->vao);
}

void resize_mesh(mesh_t * mesh, int new_size)
{
	INFO("Resizing mesh. Old size: %d, new size: %d, vertices_no: %d",
		 mesh->vertices_max, new_size, mesh->vertices_no);
	mesh->vertices_max = new_size;
	mesh->vertices =
	  reallocarray(mesh->vertices, sizeof(vec3_t), mesh->vertices_max);
	mesh->normals =
	  reallocarray(mesh->normals, sizeof(vec3_t), mesh->vertices_max);
	mesh->uvs = reallocarray(mesh->uvs, sizeof(vec2_t), mesh->vertices_max);
	mesh->indices =
	  reallocarray(mesh->indices, sizeof(GLuint), mesh->vertices_max);
	INFO("Mesh resized.");
}

void mesh_push_vertex(mesh_t * mesh, vec3_t vertex, vec2_t uv, vec3_t normal)
{
	/* Resize arrays */
	if (mesh->vertices_no >= mesh->vertices_max) {
		resize_mesh(mesh, mesh->vertices_max * 2);
	}
	memcpy(mesh->vertices + mesh->vertices_no, &vertex, sizeof(vertex));
	memcpy(mesh->uvs + mesh->vertices_no, &uv, sizeof(uv));
	memcpy(mesh->normals + mesh->vertices_no, &normal, sizeof(normal));
	mesh->indices[mesh->vertices_no] = mesh->vertices_no;
	++mesh->vertices_no;
}
