#include "boxcol.h"
#include "utilities.h"

void translate_AABB(AABB * aabb, vec3_t vec)
{
	aabb->min = v3_add(aabb->min, vec);
	aabb->max = v3_add(aabb->max, vec);
}

AABB translated_AABB(AABB * aabb, vec3_t vec)
{
	return (AABB) {
	v3_add(aabb->min, vec), v3_add(aabb->max, vec)};
}


/* This needa be fixed. */
bool collides(AABB * aabb1, AABB * aabb2)
{
	return (aabb1->max.x > aabb2->min.x &&
			aabb1->min.x < aabb2->max.x &&
			aabb1->max.y > aabb2->min.y &&
			aabb1->min.y < aabb2->max.y &&
			aabb1->max.z > aabb2->min.z && aabb1->min.z < aabb2->max.z);
}

bool map_collides(AABB * aabb, Map * map)
{
	/* Of course, this assumes that AABB contains the actual position of
	   the box itself. This could be modified by adding an additional
	   position vector, and adding it to the AABB, so then the AABB is
	   just a box. */

	/* We need to calculate all the blocks that could be in the vicinity
	   of aabb. So, if block size is 1, then... */

	for (int z = floor(aabb->min.z); z < ceil(aabb->min.z + aabb->max.z); ++z) {
		for (int y = floor(aabb->min.y); y < ceil(aabb->min.y + aabb->max.y);
			 ++y) {
			for (int x = floor(aabb->min.x);
				 x < ceil(aabb->min.x + aabb->max.x); ++x) {
				Block *b = get_block_or_null(map, x, y, z, 0);

				if (b == NULL) {
					continue;
				}

				if (b->type != 0) {
					return true;
				}
			}
		}
	}

	return false;
}
