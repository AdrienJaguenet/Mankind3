#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "graphx.h"
#include "soundfx.h"
#include "chunkmesh.h"
#include "boxcol.h"
#include "physx.h"
#include "raycast.h"
#include "movement.h"
#include "events.h"
#include "constants.h"

void attach_camera_to(Camera * camera, AABB * aabb)
{
	camera->position =
	  vec3((aabb->pos.x + aabb->dim.x / 2),
		   (aabb->pos.y + 0.75 * aabb->dim.y), (aabb->pos.z + aabb->dim.z / 2));
}

int main()
{
	INFO("Mankind %s", VERSION);
	GFXContext gfx_context;
	SFXContext sfx_context;
	init_GFX(&gfx_context, 1024, 768);
	init_SFX(&sfx_context);
	play_Audio(&sfx_context, &sfx_context.music.ambient, true, 0.0);	/* Muted... LOL */
	Map *map = calloc(sizeof(Map), 1);
	srand(SEED);
	map->height_perm = shuffled_permutations(256);
	map->wet_perm = shuffled_permutations(256);
	map->heat_perm = shuffled_permutations(256);
	preset_BiomeTable(&map->biome_table);
	InputState input_state = { false };

	for (int i = 0; i < MAX_LOD; ++i) {
		init_HashMap(&map->chunks[i]);
	}

	AABB player = { vec3(0, 10, 0), vec3(0.7, 1.9, 0.7) };
	Physics physics = { vec3(0, 0, 0), 0.5, false };

	bool running = true;
	unsigned int delta_ticks;
	unsigned int last_ticks = SDL_GetTicks();

	while (running) {
		SDL_Event event;

		unsigned int new_ticks = SDL_GetTicks();
		delta_ticks = new_ticks - last_ticks;
		last_ticks = new_ticks;

		while (SDL_PollEvent(&event)) {
			running =
			  handle_event(&event, &gfx_context.camera, &physics, map,
						   &sfx_context, delta_ticks, &input_state);
		}

		handle_input(&input_state, &gfx_context.camera, &physics);

		vec3_t collision_normal = vec3(0, 0, 0);
		update_physics(&physics, vec3(0.0, -0.0005 * delta_ticks, 0.0));
		do {
			try_move(&player, physics.velocity, map, &collision_normal);
			if (collision_normal.y != 0.f) {
				physics.velocity.y = 0;
			} else if (collision_normal.x != 0.f) {
				physics.velocity.x = 0;
				physics.touches_ground = true;
			} else if (collision_normal.z != 0.f) {
				physics.velocity.z = 0;
			}
		} while (!(collision_normal.x == 0.f || physics.velocity.x == 0.f) &&
				 !(collision_normal.y == 0.f || physics.velocity.y == 0.f) &&
				 !(collision_normal.z == 0.f || physics.velocity.z == 0.f));
		if (physics.velocity.y) {
			physics.touches_ground = true;
		}
		/* This doesn't have to be done every frame, tbh… */
		update_al_listener(player.pos, get_Camera_lookAt(&gfx_context.camera),
						   get_Camera_up(&gfx_context.camera));

		attach_camera_to(&gfx_context.camera, &player);
		gen_ChunkMesh_in_queue(&gfx_context, map, 10);
		gen_Chunks_in_queue(&gfx_context, map, 10);
		begin_draw(&gfx_context);
		draw_Map(&gfx_context, map);
		draw_UI(&gfx_context);
		end_draw(&gfx_context);
	}

	quit_GFX(&gfx_context);
	quit_SFX(&sfx_context);
	delete_Map(map);
	free(map);
	INFO("Goodbye!");
	return 0;
}
