#pragma once

#include <stdlib.h>

#include "utilities.h"

typedef struct NoiseSet {
	float height_noise, heat_noise, wet_noise, river_noise;
} NoiseSet;

typedef struct BiomeDescriptor {
	float height_min, height_max;
	float heat_min, heat_max;
	float wet_min, wet_max;
	float river_min, river_max;
} BiomeDescriptor;

typedef struct Biome {
	BiomeDescriptor descriptor;
	int type;
} Biome;

typedef struct BiomeTable {
	size_t size;
	Biome *biomes;
} BiomeTable;

void preset_BiomeTable(BiomeTable * table);
Biome *get_Biome_from_noise_or_null(NoiseSet * set, BiomeTable * table);
