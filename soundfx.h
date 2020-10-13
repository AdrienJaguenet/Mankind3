#pragma once

#include <stdbool.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "math_3d.h"
#include "utilities.h"

#define NUM_SOURCES 8

/* BIG note about OpenAL and 3D audio: it can't be stereo. */

typedef struct Audio {
	ALuint buffer;
} Audio;

typedef struct SFXContext {
	ALuint *sources;

	struct {
		Audio ambient;
	} music;

	struct {
		Audio break_block, place_block;
	} effects;
} SFXContext;

void init_SFX(SFXContext * ctx);
void load_SFX(SFXContext * ctx);
void quit_SFX(SFXContext * ctx);
void play_Audio(SFXContext * ctx, Audio * audio, bool looping);
void play_Audio_at(SFXContext * ctx, Audio * audio, vec3_t * position);
void update_al_listener(vec3_t position, vec3_t direction, vec3_t up);
