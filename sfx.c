#include "sfx.h"

void init_SFX()
{
	Mix_Init(0);
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		FATAL("SDL_mixer error: %s", Mix_GetError());	/* Maybe it's SDL_GetError()? */
	}
}

void play_audio(Mix_Chunk * sample)
{
	Mix_PlayChannel(-1, sample, 0);
}

void try_load_wav(Mix_Chunk ** chunk, const char *path)
{
	*chunk = Mix_LoadWAV(path);
	if (*chunk == NULL) {
		printf("Could not load WAV at '%s'. Reason: '%s'.\n", path,
			   Mix_GetError());
	}
}

void load_sfx(SFXContext * ctx)
{
	(void) ctx;
	/* Shit here... */
}

void clean_sfx(SFXContext * ctx)
{
	Mix_FreeChunk(ctx->effects.popped);
	Mix_FreeChunk(ctx->effects.cant_pop);
}
