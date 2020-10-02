#include "chunk.h"
#include <stdlib.h>

void randomly_populate(Chunk * chunk)
{
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				chunk->blocks[INCHUNK_INDEX(i, j, k)].type =
				  rand() % 10 < 3 ? 1 : 1;
			}
		}
	}
}
