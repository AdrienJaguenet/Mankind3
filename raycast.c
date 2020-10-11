#include "raycast.h"

bool v3_comp(vec3_t a, vec3_t b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool raycast_block(vec3_t origin, vec3_t lookat, Map * map, vec3_t * position,
				   vec3_t * normal)
{
	float _bin_size = 1.0;
	vec3_t ray_start = origin;
	vec3_t current_voxel = vec3(floor(ray_start.x / _bin_size),
								floor(ray_start.y / _bin_size),
								floor(ray_start.z / _bin_size)
	  );
	vec3_t ray_end = v3_add(origin, v3_muls(v3_norm(lookat), MAX_DISTANCE));
	vec3_t last_voxel = vec3(floor(ray_end.x / _bin_size),
							 floor(ray_end.y / _bin_size),
							 floor(ray_end.z / _bin_size)
	  );

	vec3_t ray = v3_sub(ray_end, ray_start);

	float step_x = (ray.x >= 0) ? 1 : -1;
	float step_y = (ray.y >= 0) ? 1 : -1;
	float step_z = (ray.z >= 0) ? 1 : -1;

	float next_voxel_boundary_x = (current_voxel.x + step_x) * _bin_size;
	float next_voxel_boundary_y = (current_voxel.y + step_y) * _bin_size;
	float next_voxel_boundary_z = (current_voxel.z + step_z) * _bin_size;

	float t_max_x =
	  (ray.x != 0) ? (next_voxel_boundary_x - ray_start.x) / ray.x : FLT_MAX;
	float t_max_y =
	  (ray.y != 0) ? (next_voxel_boundary_y - ray_start.y) / ray.y : FLT_MAX;
	float t_max_z =
	  (ray.z != 0) ? (next_voxel_boundary_z - ray_start.z) / ray.z : FLT_MAX;

	float t_delta_x = (ray.x != 0) ? _bin_size / ray.x * step_x : FLT_MAX;
	float t_delta_y = (ray.y != 0) ? _bin_size / ray.y * step_y : FLT_MAX;
	float t_delta_z = (ray.z != 0) ? _bin_size / ray.z * step_z : FLT_MAX;

	vec3_t diff = vec3(0, 0, 0);
	bool negative_ray = false;

	if (current_voxel.x != last_voxel.x && ray.x < 0) {
		diff.x--;
		negative_ray = true;
	}

	if (current_voxel.y != last_voxel.y && ray.y < 0) {
		diff.y--;
		negative_ray = true;
	}

	if (current_voxel.z != last_voxel.z && ray.z < 0) {
		diff.z--;
		negative_ray = true;
	}

	if (negative_ray) {
		current_voxel = v3_add(current_voxel, diff);
	}

	while (!v3_comp(last_voxel, current_voxel)) {
		if (t_max_x < t_max_y) {
			if (t_max_x < t_max_z) {
				current_voxel.x += step_x;
				t_max_x += t_delta_x;

				if (origin.x < current_voxel.x) {
					*normal = vec3(-1, 0, 0);
				} else {
					*normal = vec3(1, 0, 0);
				}
			} else {
				current_voxel.z += step_z;
				t_max_z += t_delta_z;

				if (origin.z < current_voxel.z) {
					*normal = vec3(0, 0, -1);
				} else {
					*normal = vec3(0, 0, 1);
				}
			}
		} else {
			if (t_max_y < t_max_z) {
				current_voxel.y += step_y;
				t_max_y += t_delta_y;

				if (origin.y < current_voxel.y) {
					*normal = vec3(0, -1, 0);
				} else {
					*normal = vec3(0, 1, 0);
				}
			} else {
				current_voxel.z += step_z;
				t_max_z += t_delta_z;

				if (origin.z < current_voxel.z) {
					*normal = vec3(0, 0, -1);
				} else {
					*normal = vec3(0, 0, 1);
				}
			}
		}

		Block *block = get_block_or_null(map, current_voxel.x, current_voxel.y,
										 current_voxel.z, 0);

		if (block != NULL && block->type != 0) {
			*position = current_voxel;
			return true;
		}
	}

	return false;
}
