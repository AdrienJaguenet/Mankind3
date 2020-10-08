#include "boxcol.h"
#include "utilities.h"

void translate_AABB(AABB * aabb, vec3_t vec)
{
	aabb->pos = v3_add(aabb->pos, vec);
	aabb->dim = v3_add(aabb->dim, vec);
}

AABB translated_AABB(AABB * aabb, vec3_t vec)
{
	return (AABB) {
	v3_add(aabb->pos, vec), aabb->dim};
}


/* This needa be fixed. */
bool collides(AABB * aabb1, AABB * aabb2)
{
	return (aabb1->dim.x > aabb2->pos.x &&
			aabb1->pos.x < aabb2->dim.x &&
			aabb1->dim.y > aabb2->pos.y &&
			aabb1->pos.y < aabb2->dim.y &&
			aabb1->dim.z > aabb2->pos.z && aabb1->pos.z < aabb2->dim.z);
}

bool map_collides(AABB * aabb, Map * map)
{
	/* Of course, this assumes that AABB contains the actual position of
	   the box itself. This could be modified by adding an additional
	   position vector, and adding it to the AABB, so then the AABB is
	   just a box. */

	/* We need to calculate all the blocks that could be in the vicinity
	   of aabb. So, if block size is 1, then... */

	for (int z = floor(aabb->pos.z); z < ceil(aabb->pos.z + aabb->dim.z); ++z) {
		for (int y = floor(aabb->pos.y); y < ceil(aabb->pos.y + aabb->dim.y);
			 ++y) {
			for (int x = floor(aabb->pos.x);
				 x < ceil(aabb->pos.x + aabb->dim.x); ++x) {
				Block *b = get_block_or_null(map, x, y, z, 0);

				if (b && b->type != 0) {
					return true;
				}
			}
		}
	}

	return false;
}
