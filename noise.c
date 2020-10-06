#include "noise.h"
#include <math.h>

float noise(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (float) (1.0 -
					((n * (n * n * 15731 + 789221) +
					  1376312589) & 0x7fffffff) / 1073741824.0);
}

float cubic_interpolation(float v0, float v1, float v2, float v3, float x)
{
	float p = (v3 - v2) - (v0 - v1);
	float q = (v0 - v1) - p;
	float r = v2 - v0;
	float s = v1;
	return p * x * x * x + q * x * x + r * x + s;	/* The fuck is that */
}

float cubic_nearest(float p[4], float x)
{
	return cubic_interpolation(p[0], p[1], p[2], p[3], x);
}

float noise_stretched(float x, float y, float stretch)
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
			p2[i] = noise(xw + i - 1, yw + j - 1);
		}
		/* Interpolate each row. */
		p[j] = cubic_nearest(p2, xf);
	}

	return (float) cubic_nearest(p, yf);
}

float perlin(float x, float y)
{
	float noise = 0;
	/* So, you should add to noise noise_stretched calls multiplied by
	   their INFLUENCE. The more influence they have, the more
	   influential they are. Yeah. */
	noise += noise_stretched(x, y, 40) * 8;	/* Hilly stuff. */
	noise += noise_stretched(x, y, 4) * 2;	/* A bit of detail. */
	noise += noise_stretched(x, y, 2) * 1;	/* Single block detail. */
	/* We can keep layering the stretched noise here to create more interesting terrains. */
	return noise / (8 + 2 + 1);	/* 1 + 2, because of the multipliers above. */
}
