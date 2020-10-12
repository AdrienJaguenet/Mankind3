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

bool map_collides(AABB * aabb, Map * map, vec3_t * collision_normal)
{
	/* Of course, this assumes that AABB contains the actual position of
	   the box itself. This could be modified by adding an additional
	   position vector, and adding it to the AABB, so then the AABB is
	   just a box. */

	/* We need to calculate all the blocks that could be in the vicinity
	   of aabb. So, if block size is 1, then... */

	/* We keep track of the closest block to have collided */
	vec3_t collision_point = vec3(aabb->pos.x + aabb->dim.x / 2,
								  aabb->pos.y + aabb->dim.y / 2,
								  aabb->pos.z + aabb->dim.z / 2);
	vec3_t closest_pos;
	bool collision = false;
	float closest_dist = 9999999;
	for (int z = floor(aabb->pos.z); z < ceil(aabb->pos.z + aabb->dim.z); ++z) {
		for (int y = floor(aabb->pos.y); y < ceil(aabb->pos.y + aabb->dim.y);
			 ++y) {
			for (int x = floor(aabb->pos.x);
				 x < ceil(aabb->pos.x + aabb->dim.x); ++x) {
				Block *b = get_block_or_null(map, x, y, z, 0);
				if (b && b->type != 0) {
					collision = true;
					vec3_t to_block = v3_sub(vec3(x, y, z), collision_point);
					float dist = v3_length(to_block);
					if (dist < closest_dist) {
						closest_dist = dist;
						closest_pos = to_block;
					}
				}
			}
		}
	}

	closest_pos.x = fabs(closest_pos.x);
	closest_pos.y = fabs(closest_pos.y);
	closest_pos.z = fabs(closest_pos.z);
	if (collision) {
		if (closest_pos.x > closest_pos.y) {
			if (closest_pos.x > closest_pos.z) {	/* X bigger than Y and Z */
				*collision_normal = vec3(1, 0, 0);
			} else {			/* Z is bigger than X (and therefore Y) */
				*collision_normal = vec3(0, 0, 1);
			}
		} else if (closest_pos.y > closest_pos.x) {
			if (closest_pos.y > closest_pos.z) {	/* Y bigger than either Z and Y */
				*collision_normal = vec3(0, 1, 0);
			} else {			/* Z bigger than either Y or X */
				*collision_normal = vec3(0, 0, 1);
			}
		} else {				/* Z is bigger */
			*collision_normal = vec3(0, 0, 1);
		}
	}

	return collision;
}
