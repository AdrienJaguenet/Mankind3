#include "noise.h"
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utilities.h"

int *shuffled_permutations(int size)
{
	int *hash = malloc(sizeof(int) * size);
	for (int i = 0; i < size; ++i) {
		hash[i] = i;
	}

	for (int i = 0; i < size; ++i) {
		int j = i + rand() / (RAND_MAX / (size - i) + 1);
		int t = hash[j];
		hash[j] = hash[i];
		hash[i] = t;
	}

	return hash;
}

float grad2(int hash, float x, float y)
{
	int h = hash & 7;			// Convert low 3 bits of hash code
	float u = h < 4 ? x : y;	// into 8 simple gradient directions,
	float v = h < 4 ? y : x;	// and compute the dot product with (x,y).
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float grad3(int hash, float x, float y, float z)
{
	int h = hash & 15;			// Convert low 4 bits of hash code into 12 simple
	float u = h < 8 ? x : y;	// gradient directions, and compute dot product.
	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;	// Fix repeats at h = 12 to 15
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float snoise2(float x, float y, int *perm)
{
	x += 0.5;
	y += 0.5;					/* This is to fix zero point errors... */

#define F2 0.366025403			// F2 = 0.5*(sqrt(3.0)-1.0)
#define G2 0.211324865			// G2 = (3.0-Math.sqrt(3.0))/6.0

	float n0, n1, n2;			// Noise contributions from the three corners

	// Skew the input space to determine which simplex cell we're in
	float s = (x + y) * F2;		// Hairy factor for 2D
	float xs = x + s;
	float ys = y + s;
	int i = FASTFLOOR(xs);
	int j = FASTFLOOR(ys);

	float t = (float) (i + j) * G2;
	float X0 = i - t;			// Unskew the cell origin back to (x,y) space
	float Y0 = j - t;
	float x0 = x - X0;			// The x,y distances from the cell origin
	float y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1;					// Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) {
		i1 = 1;
		j1 = 0;
	}							// lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {
		i1 = 0;
		j1 = 1;
	}							// upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2;	// Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2;	// Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 < 0.0f)
		n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad2(perm[ii + perm[jj]], x0, y0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 < 0.0f)
		n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad2(perm[ii + i1 + perm[jj + j1]], x1, y1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 < 0.0f)
		n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad2(perm[ii + 1 + perm[jj + 1]], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 40.0f * (n0 + n1 + n2);	// TODO: The scale factor is preliminary!
}

float snoise3(float x, float y, float z, int *perm)
{
	x += 0.5;
	y += 0.5;
	z += 0.5;					/* This is to fix zero point errors... */

	// Simple skewing factors for the 3D case
#define F3 0.333333333
#define G3 0.166666667

	float n0, n1, n2, n3;		// Noise contributions from the four corners

	// Skew the input space to determine which simplex cell we're in
	float s = (x + y + z) * F3;	// Very nice and simple skew factor for 3D
	float xs = x + s;
	float ys = y + s;
	float zs = z + s;
	int i = FASTFLOOR(xs);
	int j = FASTFLOOR(ys);
	int k = FASTFLOOR(zs);

	float t = (float) (i + j + k) * G3;
	float X0 = i - t;			// Unskew the cell origin back to (x,y,z) space
	float Y0 = j - t;
	float Z0 = k - t;
	float x0 = x - X0;			// The x,y,z distances from the cell origin
	float y0 = y - Y0;
	float z0 = z - Z0;

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int i1, j1, k1;				// Offsets for second corner of simplex in (i,j,k) coords
	int i2, j2, k2;				// Offsets for third corner of simplex in (i,j,k) coords

	/* This code would benefit from a backport from the GLSL version! */
	if (x0 >= y0) {
		if (y0 >= z0) {
			i1 = 1;
			j1 = 0;
			k1 = 0;
			i2 = 1;
			j2 = 1;
			k2 = 0;
		}						// X Y Z order
		else if (x0 >= z0) {
			i1 = 1;
			j1 = 0;
			k1 = 0;
			i2 = 1;
			j2 = 0;
			k2 = 1;
		}						// X Z Y order
		else {
			i1 = 0;
			j1 = 0;
			k1 = 1;
			i2 = 1;
			j2 = 0;
			k2 = 1;
		}						// Z X Y order
	} else {					// x0<y0
		if (y0 < z0) {
			i1 = 0;
			j1 = 0;
			k1 = 1;
			i2 = 0;
			j2 = 1;
			k2 = 1;
		}						// Z Y X order
		else if (x0 < z0) {
			i1 = 0;
			j1 = 1;
			k1 = 0;
			i2 = 0;
			j2 = 1;
			k2 = 1;
		}						// Y Z X order
		else {
			i1 = 0;
			j1 = 1;
			k1 = 0;
			i2 = 1;
			j2 = 1;
			k2 = 0;
		}						// Y X Z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.

	float x1 = x0 - i1 + G3;	// Offsets for second corner in (x,y,z) coords
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2.0f * G3;	// Offsets for third corner in (x,y,z) coords
	float y2 = y0 - j2 + 2.0f * G3;
	float z2 = z0 - k2 + 2.0f * G3;
	float x3 = x0 - 1.0f + 3.0f * G3;	// Offsets for last corner in (x,y,z) coords
	float y3 = y0 - 1.0f + 3.0f * G3;
	float z3 = z0 - 1.0f + 3.0f * G3;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;
	int kk = k & 0xff;

	// Calculate the contribution from the four corners
	float t0 = 0.5f - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 < 0.0f)
		n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad3(perm[ii + perm[jj + perm[kk]]], x0, y0, z0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 < 0.0f)
		n1 = 0.0f;
	else {
		t1 *= t1;
		n1 =
		  t1 * t1 * grad3(perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]], x1, y1,
						  z1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 < 0.0f)
		n2 = 0.0f;
	else {
		t2 *= t2;
		n2 =
		  t2 * t2 * grad3(perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]], x2, y2,
						  z2);
	}

	float t3 = 0.5f - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 < 0.0f)
		n3 = 0.0f;
	else {
		t3 *= t3;
		n3 =
		  t3 * t3 * grad3(perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]], x3, y3,
						  z3);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 72.0f * (n0 + n1 + n2 + n3);
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

float fractal2(float x, float y, int *permutations)
{
	return noise_layered(7, (float[2]) { snoise2(x / 400.f, y / 400.f,
												 permutations), 8.0
						 },
						 (float[2]) { snoise2(x / 100.f, y / 100.f,
											  permutations), 1.0
						 },
						 (float[2]) { snoise2(x / 70.f, y / 70.f, permutations),
						 1.0
						 },
						 (float[2]) { snoise2(x / 40.f, y / 40.f, permutations),
						 0.5
						 },
						 (float[2]) { snoise2(x / 20.f, y / 20.f, permutations),
						 0.5
						 },
						 (float[2]) { snoise2(x / 10.f, y / 10.f, permutations),
						 0.1
						 },
						 (float[2]) { snoise2(x / 6.f, y / 6.f, permutations),
						 0.2
						 }
	);
}

float fractal3(float x, float y, float z, int *permutations)
{
	return noise_layered(4,
						 (float[2]) { snoise3(x / 100.f, y / 100.f, z / 100.f,
											  permutations), 4.0
						 },
						 (float[2]) { snoise3(x / 80.f, y / 80.f, z / 80.f,
											  permutations), 1.0
						 },
						 (float[2]) { snoise3(x / 30.f, y / 30.f, z / 30.f,
											  permutations), 1.0
						 },
						 (float[2]) { snoise3(x / 20.f, y / 20.f, z / 20.f,
											  permutations), 0.5
						 }
	);
}
