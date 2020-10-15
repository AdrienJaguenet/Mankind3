#include "biome.h"

void preset_BiomeTable(BiomeTable * table)
{
	table->size = 12;

	Biome water;
	water.type = 6;
	water.descriptor = (BiomeDescriptor) {
	-1, -0.2, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0};

	Biome polar;
	polar.type = 5;
	polar.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, -1.0, -0.66, -0.2, 0.2, -1.0, 1.0};

	Biome tundra;
	tundra.type = 2;
	tundra.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, -0.66, -0.33, -0.4, 0.4, -1.0, 1.0};

	Biome boreal_forest;
	tundra.type = 2;
	boreal_forest.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, -0.33, 0.0, -0.6, 0.6, -1.0, 1.0};

	Biome cold_desert;
	cold_desert.type = 4;
	cold_desert.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.0, 0.33, -0.8, -0.3, -1.0, 1.0};

	Biome prairie;
	prairie.type = 2;
	prairie.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.0, 0.33, -0.3, 0.2, -1.0, 1.0};

	Biome temperate_deciduous;
	temperate_deciduous.type = 2;
	temperate_deciduous.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.0, 0.33, 0.2, 0.8, -1.0, 1.0};

	Biome warm_desert;
	warm_desert.type = 4;
	warm_desert.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.33, 1.0, -1.0, -0.6, -1.0, 1.0};

	Biome tropical_grassland;
	tropical_grassland.type = 2;
	tropical_grassland.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.33, 1.0, -0.6, -0.2, -1.0, 1.0};

	Biome savanna;
	savanna.type = 2;
	savanna.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.33, 1.0, -0.2, 0.2, -1.0, 1.0};

	Biome tropical_deciduous;
	tropical_deciduous.type = 2;
	tropical_deciduous.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.33, 1.0, 0.2, 0.6, -1.0, 1.0};

	Biome tropical_rainforest;
	tropical_rainforest.type = 2;
	tropical_rainforest.descriptor = (BiomeDescriptor) {
	-0.2, 1.0, 0.33, 1.0, 0.6, 1.0, -1.0, 1.0};

	table->biomes = malloc(sizeof(Biome) * table->size);
	table->biomes[0] = water;
	table->biomes[1] = polar;
	table->biomes[2] = tundra;
	table->biomes[3] = boreal_forest;
	table->biomes[4] = cold_desert;
	table->biomes[5] = prairie;
	table->biomes[6] = temperate_deciduous;
	table->biomes[7] = warm_desert;
	table->biomes[8] = tropical_grassland;
	table->biomes[9] = savanna;
	table->biomes[10] = tropical_deciduous;
	table->biomes[11] = tropical_rainforest;
}

Biome *get_Biome_from_noise_or_null(NoiseSet * set, BiomeTable * table)
{
	set->heat_noise = (set->heat_noise * 2 + set->height_noise * 3) / (2 + 3);
	set->wet_noise = (set->wet_noise * 1 + set->height_noise * 1) / (1 + 1);
	for (size_t i = 0; i < table->size; ++i) {
		Biome *biome = &table->biomes[i];
		BiomeDescriptor descriptor = biome->descriptor;

		if (set->height_noise >= descriptor.height_min
			&& set->height_noise < descriptor.height_max
			&& set->heat_noise >= descriptor.heat_min
			&& set->heat_noise < descriptor.heat_max
			&& set->wet_noise >= descriptor.wet_min
			&& set->wet_noise < descriptor.wet_max) {
			return biome;
		}
	}

	return NULL;
}
