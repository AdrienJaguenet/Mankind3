#pragma once

#include <stdbool.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "math_3d.h"
#include "utilities.h"

/* BIG note about OpenAL and 3D audio: it can't be stereo. */

typedef struct Audio {
	bool global;
	vec3_t origin;
	ALuint buffer, source;
} Audio;

typedef struct SFXContext {
	struct {
		Audio break_block, place_block;
	} effects;
} SFXContext;

void init_SFX(SFXContext * ctx);
void load_SFX(SFXContext * ctx);
void quit_SFX(SFXContext * ctx);
void play_Audio(Audio * audio);
void play_Audio_at(Audio * audio, vec3_t * position);
void update_al_listener(vec3_t position, vec3_t direction, vec3_t up);
