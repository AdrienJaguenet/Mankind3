#include "chunkmesh.h"
#include <stdlib.h>

void push_face(mesh_t * mesh, vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3,
			   vec3_t norm, int type)
{
	vec2_t uv_corners[4];
	get_tex_quad(mesh->texture, type, 128, uv_corners);

	mesh_push_vertex(mesh, v0, uv_corners[UV_TOP_LEFT], norm);
	mesh_push_vertex(mesh, v1, uv_corners[UV_TOP_RIGHT], norm);
	mesh_push_vertex(mesh, v2, uv_corners[UV_BOTTOM_LEFT], norm);
	//--
	mesh_push_vertex(mesh, v3, uv_corners[UV_BOTTOM_RIGHT], norm);
	mesh_push_vertex(mesh, v2, uv_corners[UV_BOTTOM_LEFT], norm);
	mesh_push_vertex(mesh, v1, uv_corners[UV_TOP_RIGHT], norm);
}

void generate_chunk_mesh(Chunk * chunk, Map * map, Texture * tilemap)
{
	chunk->dirty = false;
	if (chunk->mesh) {
		mesh_terminate(chunk->mesh);
	}
	if (chunk->empty){
	  chunk->mesh = NULL;
	  return;
	}

	chunk->mesh = calloc(sizeof(mesh_t), 1);
	resize_mesh(chunk->mesh, 512);
	chunk->mesh->texture = tilemap;

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				Block *b = chunk->blocks + INCHUNK_INDEX(i, j, k);
				if (!b->type) {
					continue;
				}
				vec3_t vertex_alias[8] = {
					vec3(i, j, k),	// 0
					vec3(i, j, k + 1),	// 1
					vec3(i, j + 1, k),	// 2
					vec3(i, j + 1, k + 1),	// 3
					vec3(i + 1, j, k),	// 4
					vec3(i + 1, j, k + 1),	// 5
					vec3(i + 1, j + 1, k),	// 6
					vec3(i + 1, j + 1, k + 1),	// 7
				};
				Block *neighbourhood[6] = { NULL };
				get_neighbourhood(map, i + chunk->x * CHUNK_SIZE,
								  j + chunk->y * CHUNK_SIZE,
								  k + chunk->z * CHUNK_SIZE, neighbourhood);
				Block *right = neighbourhood[NEIGHBOUR_RIGHT], *left =
				  neighbourhood[NEIGHBOUR_LEFT], *up =
				  neighbourhood[NEIGHBOUR_UP], *down =
				  neighbourhood[NEIGHBOUR_DOWN], *front =
				  neighbourhood[NEIGHBOUR_FRONT], *back =
				  neighbourhood[NEIGHBOUR_BACK];
				if (left && !left->type) {
					/* -X or left face
					   3 2
					   1 0
					 */
					push_face(chunk->mesh, vertex_alias[3],
							  vertex_alias[2],
							  vertex_alias[1],
							  vertex_alias[0], vec3(-1, 0, 0), b->type);
				}

				if (right && !right->type) {
					/* +X or right face
					   6 7
					   4 5
					 */
					push_face(chunk->mesh, vertex_alias[6],
							  vertex_alias[7],
							  vertex_alias[4],
							  vertex_alias[5], vec3(+1, 0, 0), b->type);
				}

				if (front && !front->type) {
					/* -Z or front face
					   2 6
					   0 4
					 */
					push_face(chunk->mesh, vertex_alias[2],
							  vertex_alias[6],
							  vertex_alias[0],
							  vertex_alias[4], vec3(0, 0, -1), b->type);
				}

				if (back && !back->type) {
					/* +Z or back face
					   7 3
					   5 1
					 */
					push_face(chunk->mesh, vertex_alias[7],
							  vertex_alias[3],
							  vertex_alias[5],
							  vertex_alias[1], vec3(0, 0, +1), b->type);
				}

				if (down && !down->type) {
					/* -Y or bottom face
					   0 4
					   1 5
					 */
					push_face(chunk->mesh, vertex_alias[0],
							  vertex_alias[4],
							  vertex_alias[1],
							  vertex_alias[5], vec3(0, -1, 0), b->type);
				}

				if (up && !up->type) {
					/* +Y or top face
					   3 7
					   2 6
					 */
					push_face(chunk->mesh, vertex_alias[3],
							  vertex_alias[7],
							  vertex_alias[2],
							  vertex_alias[6], vec3(0, +1, 0), b->type);
				}
			}
		}
	}
	mesh_load(chunk->mesh);
}
