#include "mesh.h"

#include <stdlib.h>
#include <string.h>

void mesh_load(mesh_t * mesh)
{
	/* Generate the VAO. */
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	/* Generate the buffer objects.. */
	glGenBuffers(1, &mesh->pbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->pbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices_no * sizeof(GLuint),
				 mesh->packed, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->vertices_no * sizeof(GLuint),
				 mesh->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->pbo);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, (void *) 0);
}

void mesh_render(mesh_t * mesh, Texture* texture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->index);

	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	glDrawElements(GL_TRIANGLES, mesh->vertices_no, GL_UNSIGNED_INT,
				   (void *) 0);

	glBindVertexArray(0);
}

void mesh_terminate(mesh_t * mesh)
{
	free(mesh->packed);
	free(mesh->indices);
	glDeleteBuffers(1, &mesh->pbo);
	glDeleteBuffers(1, &mesh->ebo);

	glDeleteVertexArrays(1, &mesh->vao);
	free(mesh);
}

void resize_mesh(mesh_t * mesh, int new_size)
{
	mesh->vertices_max = new_size;
	mesh->packed =
	  reallocarray(mesh->packed, sizeof(GLuint), mesh->vertices_max);
	mesh->indices =
	  reallocarray(mesh->indices, sizeof(GLuint), mesh->vertices_max);
}

void mesh_push_vertex(mesh_t * mesh, ECorner corner, EFace face,
					  vec3_t position, int type)
{
	/* Resize arrays */
	if (mesh->vertices_no >= mesh->vertices_max) {
		resize_mesh(mesh, mesh->vertices_max * 2);
	}
	GLuint pack = ((face & FACE_MASK) << FACE_OFFSET) |
	  (((GLuint) position.x & POS_MASK) << POSX_OFFSET) |
	  (((GLuint) position.y & POS_MASK) << POSY_OFFSET) |
	  (((GLuint) position.z & POS_MASK) << POSZ_OFFSET) |
	  ((corner & CORNER_MASK) << CORNER_OFFSET) |
	  ((type & TYPE_MASK) << TYPE_OFFSET);

	mesh->packed[mesh->vertices_no] = pack;
	mesh->indices[mesh->vertices_no] = mesh->vertices_no;
	++mesh->vertices_no;
}
