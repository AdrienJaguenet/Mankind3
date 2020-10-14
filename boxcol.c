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

bool map_collides(AABB * aabb, Map * map, vec3_t * collision_normal,
				  bool enabled_faces[6])
{
	/* Of course, this assumes that AABB contains the actual position of
	   the box itself. This could be modified by adding an additional
	   position vector, and adding it to the AABB, so then the AABB is
	   just a box. */

	/* We need to calculate all the blocks that could be in the vicinity
	   of aabb. So, if block size is 1, then... */

	/* We keep track of the closest block to have collided */
	vec3_t collision_points[6];
	collision_points[FACE_LEFT] = vec3(aabb->pos.x,
									   aabb->pos.y + aabb->dim.y / 2,
									   aabb->pos.z + aabb->dim.z / 2);
	collision_points[FACE_RIGHT] = vec3(aabb->pos.x + aabb->dim.x,
										aabb->pos.y + aabb->dim.y / 2,
										aabb->pos.z + aabb->dim.z / 2);
	collision_points[FACE_DOWN] = vec3(aabb->pos.x + aabb->dim.x / 2,
									   aabb->pos.y,
									   aabb->pos.z + aabb->dim.z / 2);
	collision_points[FACE_UP] = vec3(aabb->pos.x + aabb->dim.x / 2,
									 aabb->pos.y + aabb->dim.y,
									 aabb->pos.z + aabb->dim.z / 2);
	collision_points[FACE_BACK] = vec3(aabb->pos.x + aabb->dim.x / 2,
									   aabb->pos.y + aabb->dim.y / 2,
									   aabb->pos.z);
	collision_points[FACE_FRONT] = vec3(aabb->pos.x + aabb->dim.x / 2,
										aabb->pos.y + aabb->dim.y / 2,
										aabb->pos.z + aabb->dim.z);
	bool collision = false;
	float closest_dist = 9999999;
	int closest_face = FACE_BACK;
	for (int z = floor(aabb->pos.z); z < ceil(aabb->pos.z + aabb->dim.z); ++z) {
		for (int y = floor(aabb->pos.y); y < ceil(aabb->pos.y + aabb->dim.y);
			 ++y) {
			for (int x = floor(aabb->pos.x);
				 x < ceil(aabb->pos.x + aabb->dim.x); ++x) {
				Block *b = get_block_or_null(map, x, y, z, 0);
				if (b && b->type != 0) {
					collision = true;
					for (int face = 0; face < 6; ++face) {
						if (!enabled_faces[face]) {
							continue;
						}
						float dist = 0.f;
						switch (face) {
							  /* Z axis, compare against the BACK face of the cube */
						  case FACE_BACK:
							  dist = fabs(collision_points[face].z - z);
							  break;

						  case FACE_FRONT:
							  dist =
								fabs(collision_points[face].z -
									 (z + BLOCK_SIZE));
							  break;

						  case FACE_RIGHT:
							  dist = fabs(collision_points[face].x - x);
							  break;

						  case FACE_LEFT:
							  dist =
								fabs(collision_points[face].x -
									 (x + BLOCK_SIZE));
							  break;

						  case FACE_UP:
							  dist = fabs(collision_points[face].y - y);
							  break;

						  case FACE_DOWN:
							  dist =
								fabs(collision_points[face].y -
									 (y + BLOCK_SIZE));
							  break;
						}
						if (dist < closest_dist) {
							closest_dist = dist;
							closest_face = face;
						}
					}
				}
			}
		}
	}
	*collision_normal = FACE_NORMALS[closest_face];
	return collision;
}
