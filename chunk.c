#include "chunk.h"
#include <stdlib.h>

int get_height(int x, int z)
{
	return CHUNK_SIZE / 2 + (x + z) % (CHUNK_SIZE / 2);
}

void randomly_populate(Chunk * chunk)
{
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int k = 0; k < CHUNK_SIZE; ++k) {
			int height = get_height(i, k);
			for (int j = 0; j < CHUNK_SIZE; ++j) {
			  int type = 0;
			  if (j < height - 4) {
				type = 1;
			  } else if (j < height - 1) {
				type = 3;
			  } else if (j == height - 1) {
				type = 2;
			  }
			  chunk->blocks[INCHUNK_INDEX(i, j, k)].type = type;
			}
		}
	}
}
