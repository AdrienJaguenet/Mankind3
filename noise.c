#include "noise.h"
#include <math.h>
#include <stdlib.h>

const int hash_size = 256;
const int hash_mask = 255;

int *shuffled_hash()
{
	int *hash = malloc(sizeof(int) * hash_size);
	for (int i = 0; i < hash_size; ++i) {
		hash[i] = i;
	}

	for (int i = 0; i < hash_size; ++i) {
		int j = i + rand() / (RAND_MAX / (hash_size - i) + 1);
		int t = hash[j];
		hash[j] = hash[i];
		hash[i] = t;
	}

	return hash;
}

/* Returns a random float between -1 and 1 that's predictable. */

float noise(int x, int y, int *hash)
{
	x &= hash_mask;
	y &= hash_mask;
	return hash[(hash[x] + y) & hash_mask] * (2.0 / hash_mask) - 1.0;
}

float cubic_interpolation(float p[4], float x)
{
	float P = (p[3] - p[2]) - (p[0] - p[1]);
	float Q = (p[0] - p[1]) - P;
	float R = p[2] - p[0];
	float S = p[1];
	return P * x * x * x + Q * x * x + R * x + S;	/* The fuck is that */
}

float noise_stretched(float x, float y, float stretch, int *hash)
{
	// stretch
	x /= stretch;
	y /= stretch;
	/* Whole numbers. */
	int xw = floor(x);
	int yw = floor(y);
	/* Fractional parts. */
	float xf = x - xw;
	float yf = y - yw;

	float p[4] = { 0 };

	for (int j = 0; j < 4; j++) {
		float p2[4] = { 0 };
		for (int i = 0; i < 4; i++) {
			p2[i] = noise(xw + i - 1, yw + j - 1, hash);
		}
		/* Interpolate each row. */
		p[j] = cubic_interpolation(p2, xf);
	}

	return (float) cubic_interpolation(p, yf);
}

float perlin(float x, float y, int *hash)
{
	float noise = 0;
	/* So, you should add to noise noise_stretched calls multiplied by
	   their INFLUENCE. The more influence they have, the more
	   influential they are. Yeah. */
	noise += noise_stretched(x, y, 160, hash) * 10;	/* Hilly stuff. */
	noise += noise_stretched(x, y, 80, hash) * 8;	/* Hilly stuff. */
	noise += noise_stretched(x, y, 20, hash) * 2;	/* Hilly stuff. */
	noise += noise_stretched(x, y, 8, hash) * 1;	/* Hilly stuff. */
	noise += noise_stretched(x, y, 3, hash) * 0.5;	/* Hilly stuff. */
	/* We can keep layering the stretched noise here to create more interesting terrains. */
	return noise / (10 + 8 + 2 + 1 + 0.5);	/* 1 + 2, because of the multipliers above. */
}
