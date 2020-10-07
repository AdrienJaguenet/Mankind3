#include "noise.h"
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utilities.h"

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

float noise_layered(int count, ...)
{
	va_list ap;

	float noise = 0;
	float influence_total = 0;
	va_start(ap, count);

	for (int i = 0; i < count; ++i) {
		float *ns = va_arg(ap, float *);
		noise += ns[0] * ns[1];
		influence_total += ns[1];
	}

	va_end(ap);

	return noise / influence_total;
}

float fractal(float x, float y, int *hash)
{
	return noise_layered(7,
						 (float[2]) { noise_stretched(x, y, 400, hash), 8.0 },
						 (float[2]) { noise_stretched(x, y, 100, hash), 1.0 },
						 (float[2]) { noise_stretched(x, y, 70, hash), 1.0 },
						 (float[2]) { noise_stretched(x, y, 40, hash), 0.5 },
						 (float[2]) { noise_stretched(x, y, 20, hash), 0.5 },
						 (float[2]) { noise_stretched(x, y, 10, hash), 0.1 },
						 (float[2]) { noise_stretched(x, y, 6, hash), 0.2 }
	);
}
