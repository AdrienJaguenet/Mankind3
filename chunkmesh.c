#include "chunkmesh.h"

typedef struct {
	float x, y;
} vec2_t;

void push_vertex(mesh_t * mesh, vec3_t vertex, vec2_t uv, vec3_t normal)
{
	(void) mesh;
	(void) vertex;
	(void) uv;
	(void) normal;
}

void push_face(mesh_t * mesh, vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3,
			   vec3_t norm, int type)
{
	(void) type;
	vec2_t top_right = { 0.0f, 0.0f },
	  top_left = { 1.0f, 0.0f },
	  bottom_left = { 0.0f, 1.0f }, bottom_right = { 1.0f, 1.0f };

	push_vertex(mesh, v0, top_left, norm);
	push_vertex(mesh, v1, top_right, norm);
	push_vertex(mesh, v2, bottom_left, norm);
	//--
	push_vertex(mesh, v3, bottom_right, norm);
	push_vertex(mesh, v2, bottom_left, norm);
	push_vertex(mesh, v1, top_right, norm);
}

void generate_chunk_mesh(Chunk * chunk, Map * map)
{
	vec3_t vertices[(CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1)];
	for (int i = 0; i <= CHUNK_SIZE; ++i) {
		for (int j = 0; j <= CHUNK_SIZE; ++j) {
			for (int k = 0; k <= CHUNK_SIZE; ++k) {
				vertices[INDEX(i, j, k)] = vec3(i, j, k);
			}
		}
	}

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				Block *b = chunk->blocks + INDEX(i, j, k);
				vec3_t vertex_alias[8] = {
					vertices[INDEX(i, j, k)],	// 0
					vertices[INDEX(i, j, k + 1)],	// 1
					vertices[INDEX(i, j + 1, k)],	// 2
					vertices[INDEX(i, j + 1, k + 1)],	// 3
					vertices[INDEX(i + 1, j, k)],	// 4
					vertices[INDEX(i + 1, j, k + 1)],	// 5
					vertices[INDEX(i + 1, j + 1, k)],	// 6
					vertices[INDEX(i + 1, j + 1, k + 1)],	// 7
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

}
