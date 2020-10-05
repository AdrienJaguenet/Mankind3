#pragma once
#include "map.h"
#include "texture.h"

#define INDEX(x, y, z) ((x) * (CHUNK_SIZE+1) * (CHUNK_SIZE+1) + (y) * (CHUNK_SIZE+1) + (z))

void generate_chunk_mesh(Chunk * chunk, Map * map, Texture * tilemap);
