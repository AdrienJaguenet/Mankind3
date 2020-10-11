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
		INFO("Could not load WAV at '%s': %s", path, Mix_GetError());
	}
}

void load_SFX(SFXContext * ctx)
{
	try_load_wav(&ctx->effects.break_block, "resources/sfx/break_block.wav");
	try_load_wav(&ctx->effects.place_block, "resources/sfx/place_block.wav");
}

void clean_SFX(SFXContext * ctx)
{
	Mix_FreeChunk(ctx->effects.break_block);
	Mix_FreeChunk(ctx->effects.place_block);
}
