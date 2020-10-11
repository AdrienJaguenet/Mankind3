#include "soundfx.h"

void load_Audio(Audio * audio, const char *path)
{
	audio->buffer = alutCreateBufferFromFile(path);

	if (audio->buffer == AL_NONE) {
		FATAL("ALUT error: %s", alutGetErrorString(alutGetError()));
	}

	alGenSources(1, &audio->source);
	alSourcei(audio->source, AL_BUFFER, audio->buffer);
}

void play_Audio(Audio * audio)
{
	alSourcePlay(audio->source);
}

void delete_Audio(Audio * audio)
{
	alDeleteSources(1, &audio->source);
	alDeleteBuffers(1, &audio->buffer);
}

void init_SFX(SFXContext * ctx)
{
	alutInit(0, NULL);
	alGetError();

	load_SFX(ctx);
}

void load_SFX(SFXContext * ctx)
{
	load_Audio(&ctx->effects.break_block, "resources/sfx/break_block16.wav");
	load_Audio(&ctx->effects.place_block, "resources/sfx/place_block16.wav");
}

void quit_SFX(SFXContext * ctx)
{
	alutExit();
	delete_Audio(&ctx->effects.break_block);
	delete_Audio(&ctx->effects.place_block);
}
