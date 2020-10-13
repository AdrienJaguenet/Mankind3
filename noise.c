#include "noise.h"

#define FADE(t) (t * t * t * (t * (t * 6 - 15) + 10))
#define LERP(t, a, b) (a + t * (b - a))

char *shuffled_permutations(size_t size)
{
	char *hash = malloc(sizeof(char) * size);
	for (size_t i = 0; i < size; ++i) {
		hash[i] = i;
	}

	for (size_t i = 0; i < size - 1; ++i) {
		size_t j = i + rand() / (RAND_MAX / (size - i) + 1);
		char t = hash[j];
		hash[j] = hash[i];
		hash[i] = t;
	}

	return hash;
}

static float GRAD2(int hash, float x, float y)
{
	return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
}

static float GRAD3(int hash, float x, float y, float z)
{
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float noise2(float x, float y, char *permutations)
{
	int X = (int) floor(x) & 0xff;
	int Y = (int) floor(y) & 0xff;
	x -= floor(x);
	y -= floor(y);
	float u = FADE(x);
	float v = FADE(y);
	int A = (permutations[X] + Y) & 0xff;
	int B = (permutations[X + 1] + Y) & 0xff;
	return LERP(v,
				LERP(u, GRAD2(permutations[A], x, y),
					 GRAD2(permutations[B], x - 1, y)), LERP(u,
															 GRAD2(permutations
																   [A + 1], x,
																   y - 1),
															 GRAD2(permutations
																   [B + 1],
																   x - 1,
																   y - 1)));
}

float noise3(float x, float y, float z, char *permutations)
{
	int X = (int) floor(x) & 0xff;
	int Y = (int) floor(y) & 0xff;
	int Z = (int) floor(z) & 0xff;
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	float u = FADE(x);
	float v = FADE(y);
	float w = FADE(z);
	int A = (permutations[X] + Y) & 0xff;
	int B = (permutations[X + 1] + Y) & 0xff;
	int AA = (permutations[A] + Z) & 0xff;
	int BA = (permutations[B] + Z) & 0xff;
	int AB = (permutations[A + 1] + Z) & 0xff;
	int BB = (permutations[B + 1] + Z) & 0xff;
	return LERP(w,
				LERP(v,
					 LERP(u, GRAD3(permutations[AA], x, y, z),
						  GRAD3(permutations[BA], x - 1, y, z)), LERP(u,
																	  GRAD3
																	  (permutations
																	   [AB], x,
																	   y - 1,
																	   z),
																	  GRAD3
																	  (permutations
																	   [BB],
																	   x - 1,
																	   y - 1,
																	   z))),
				LERP(v,
					 LERP(u, GRAD3(permutations[AA + 1], x, y, z - 1),
						  GRAD3(permutations[BA + 1], x - 1, y, z - 1)), LERP(u,
																			  GRAD3
																			  (permutations
																			   [AB
																				+
																				1],
																			   x,
																			   y
																			   -
																			   1,
																			   z
																			   -
																			   1),
																			  GRAD3
																			  (permutations
																			   [BB
																				+
																				1],
																			   x
																			   -
																			   1,
																			   y
																			   -
																			   1,
																			   z
																			   -
																			   1))));
}

float fbm2(float x, float y, int octave, char *permutations)
{
	float f = 0.0;
	float w = 0.5;
	for (int i = 0; i < octave; i++) {
		f += w * noise2(x, y, permutations);
		x *= 2.0;
		y *= 2.0;
		w *= 0.5;
	}
	return f;
}

float fbm3(float x, float y, float z, int octave, char *permutations)
{
	float f = 0.0;
	float w = 0.5;
	for (int i = 0; i < octave; i++) {
		f += w * noise3(x, y, z, permutations);
		x *= 2.0;
		y *= 2.0;
		z *= 2.0;
		w *= 0.5;
	}
	return f;
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

/* With the copypasted noise shit, stretching now works by dividing
	the x and y values. Kinda cool, tbh. */

float fractal2(float x, float y, char *permutations)
{
	return fbm2(x / 400.f, y / 400.f, 7, permutations);
}

float fractal3(float x, float y, float z, char *permutations)
{
	return noise3(x / 50.f, y / 50.f, z / 50.f, permutations);
}
