#include "chunk.h"
#include <stdlib.h>

int get_height(int x, int z)
{
	(void) x;
	(void) z;
	return 10 + rand() % 10;
	//return CHUNK_SIZE / 2 + (x + z) % (CHUNK_SIZE / 2) - rand() % 3;
}

void randomly_populate(Chunk * chunk)
{
	int base_height = chunk->y * CHUNK_SIZE;
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int k = 0; k < CHUNK_SIZE; ++k) {
			int height = get_height(i, k);
			for (int j = 0; j < CHUNK_SIZE; ++j) {
				int type = 0;
				if (j + base_height < height - 4) {
					type = 1;
				} else if (j + base_height < height - 1) {
					type = 3;
				} else if (j + base_height == height - 1) {
					type = 2;
				}
				set_Chunk_block_type(chunk, i, j, k, type);
			}
		}
	}
}

void set_Chunk_block_type(Chunk * c, int x, int y, int z, int type)
{
	c->dirty = true;
	if (type) {
		c->empty = false;
	}
	c->blocks[INCHUNK_INDEX(x, y, z)].type = type;
}
