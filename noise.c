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
	return noise_layered(7, (float[2]) { noise2(x / 400.f, y / 400.f,
												permutations), 8.0
						 },
						 (float[2]) { noise2(x / 100.f, y / 100.f,
											 permutations), 1.0
						 },
						 (float[2]) { noise2(x / 70.f, y / 70.f, permutations),
						 1.0
						 },
						 (float[2]) { noise2(x / 40.f, y / 40.f, permutations),
						 0.5
						 },
						 (float[2]) { noise2(x / 20.f, y / 20.f, permutations),
						 0.5
						 },
						 (float[2]) { noise2(x / 10.f, y / 10.f, permutations),
						 0.1
						 },
						 (float[2]) { noise2(x / 6.f, y / 6.f, permutations),
						 0.2
						 }
	);
}

float fractal3(float x, float y, float z, char *permutations)
{
	return noise_layered(4, (float[2]) { noise3(x / 100.f, y / 100.f, z / 100.f,
												permutations), 3.0
						 },
						 (float[2]) { noise3(x / 80.f, y / 80.f, z / 80.f,
											 permutations), 2.0
						 },
						 (float[2]) { noise3(x / 30.f, y / 30.f, z / 30.f,
											 permutations), 4.0
						 },
						 (float[2]) { noise3(x / 20.f, y / 20.f, z / 20.f,
											 permutations), 8.5
						 }
	);
}
