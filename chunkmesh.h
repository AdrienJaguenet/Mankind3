#pragma once
#include "map.h"
#include "texture.h"

#define INDEX(x, y, z) ((x) * (CHUNK_SIZE+1) * (CHUNK_SIZE+1) + (y) * (CHUNK_SIZE+1) + (z))

void push_face(mesh_t * mesh, vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3,
			   vec3_t norm, int type);

void generate_chunk_mesh(Chunk * chunk, Map * map, Texture * tilemap);
