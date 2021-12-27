#pragma once
#include "soundfx.h"
#include "camera.h"
#include "physx.h"
#include "map.h"

bool handle_event(SDL_Event * e, Camera * camera, Physics * physics, Map * map,
				  SFXContext * sfx_context, unsigned int delta_ticks);
