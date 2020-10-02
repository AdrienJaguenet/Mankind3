#include "chunk.h"
#include <stdlib.h>

void randomly_populate(Chunk * chunk)
{
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				chunk->blocks[INCHUNK_INDEX(i, j, k)].type =
				  rand() % 10 - j > 0 && i > 0 && j > 0 && k > 0
				  && i < CHUNK_SIZE - 1 && j < CHUNK_SIZE - 1
				  && k < CHUNK_SIZE - 1 ? 1 : 0;
			}
		}
	}
}
