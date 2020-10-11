#include "soundfx.h"

void load_Audio(Audio * audio, const char *path)
{
	audio->buffer = alutCreateBufferFromFile(path);

	int error = alGetError();

	if (error != AL_NO_ERROR) {
		FATAL("OpenAL error.");
	}

	alGenSources(1, &audio->source);
	alSourcei(audio->source, AL_BUFFER, audio->buffer);
}

void load_helloworld(Audio * audio)
{
	audio->buffer = alutCreateBufferHelloWorld();
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
	/* load_Audio(&ctx->effects.break_block, "resources/sfx/random3.wav"); */
	/* load_Audio(&ctx->effects.place_block, "resources/sfx/random3.wav"); */
	load_helloworld(&ctx->effects.break_block);
	load_helloworld(&ctx->effects.place_block);
}

void quit_SFX(SFXContext * ctx)
{
	alutExit();
	delete_Audio(&ctx->effects.break_block);
	delete_Audio(&ctx->effects.place_block);
}
