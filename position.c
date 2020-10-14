#include "position.h"

#include <math.h>

const vec3_t FACE_NORMALS[6] = {
	{.m = { -1, 0, 0} },
	{.m = { 1, 0, 0} },
	{.m = { 0, 1, 0} },
	{.m = { 0, -1, 0} },
	{.m = { 0, 0, -1} },
	{.m = { 0, 0, 1} }
};


void get_chunk_pos(int px, int py, int pz, int *cx, int *cy, int *cz)
{
	*cx = floor((float) px / CHUNK_SIZE);
	*cy = floor((float) py / CHUNK_SIZE);
	*cz = floor((float) pz / CHUNK_SIZE);
}

void get_pos_in_chunk(int px, int py, int pz, int *bx, int *by, int *bz,
					  int lod)
{
	*bx = (((px % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE) >> lod;
	*by = (((py % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE) >> lod;
	*bz = (((pz % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE) >> lod;
}
