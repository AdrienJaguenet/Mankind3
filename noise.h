#pragma once

float noise(int x, int y);
float cubic_nearest(float p[4], float x);
float noise_stretched(float x, float y, float stretch);
float cubic_interpolation(float v0, float v1, float v2, float v3, float x);
float perlin(float x, float y);
