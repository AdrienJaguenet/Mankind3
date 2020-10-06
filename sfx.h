#pragma once

#include <SDL2/SDL_mixer.h>

#include "utilities.h"

typedef struct {
	struct {
		Mix_Chunk *popped;
		Mix_Chunk *cant_pop;
	} effects;
} SFXContext;

void init_SFX();
void play_audio(Mix_Chunk * sample);
void try_load_wav(Mix_Chunk ** chunk, const char *path);
void load_sfx(SFXContext * ctx);
void clean_sfx(SFXContext * ctx);
