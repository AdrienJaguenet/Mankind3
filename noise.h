#pragma once

int *shuffled_hash();
float noise(int x, int y, int *hash);
float cubic_interpolation(float p[4], float x);
float noise_stretched(float x, float y, float stretch, int *hash);
float noise_layered(int count, ...);
float fractal(float x, float y, int *hash);
