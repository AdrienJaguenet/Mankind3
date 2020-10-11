#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "utilities.h"

typedef struct Audio {
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
