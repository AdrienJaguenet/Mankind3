#include "movement.h"

bool try_move(AABB * aabb, vec3_t diff, Map * map, vec3_t * collision_normal)
{
	/* Calculate the length only once */
	float diff_l = v3_length(diff);
	bool enabled_faces[6] = { false };
	if (diff.x > 0) {
		enabled_faces[FACE_RIGHT] = true;
	} else if (diff.x < 0) {
		enabled_faces[FACE_LEFT] = true;
	}

	if (diff.y > 0) {
		enabled_faces[FACE_UP] = true;
	} else if (diff.y < 0) {
		enabled_faces[FACE_DOWN] = true;
	}

	if (diff.z > 0) {
		enabled_faces[FACE_FRONT] = true;
	} else if (diff.z < 0) {
		enabled_faces[FACE_BACK] = true;
	}

	do {
		AABB translated = translated_AABB(aabb, diff);
		if (!map_collides(&translated, map, collision_normal, enabled_faces)) {
			*aabb = translated;
			return true;
		} else {
			/* try again by moving only halfway */
			diff = v3_divs(diff, 2.f);
			diff_l /= 2.f;
		}
	} while (diff_l > 0.001);
	return false;

}
