#include "soundfx.h"

void load_Audio(Audio * audio, const char *path, bool looping)
{
	audio->buffer = alutCreateBufferFromFile(path);

	if (audio->buffer == AL_NONE) {
		FATAL("ALUT error: %s", alutGetErrorString(alutGetError()));
	}

	alGenSources(1, &audio->source);
	alSourcei(audio->source, AL_BUFFER, audio->buffer);
	alSourcei(audio->source, AL_LOOPING, looping);
	alSource3f(audio->source, AL_VELOCITY, 0.0, 0.0, 0.0);
}

void play_Audio(Audio * audio)
{
	alSourcePlay(audio->source);
}

void play_Audio_at(Audio * audio, vec3_t * position)
{
	alSource3f(audio->source, AL_POSITION, position->x, position->y,
			   position->z);
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

/* Soon, vec3_t velocity */
void update_al_listener(vec3_t position, vec3_t direction, vec3_t up)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
	alListenerfv(AL_ORIENTATION, (ALfloat[6]) {
				 direction.x, direction.y, direction.z, up.x, up.y, up.z});
}

void load_SFX(SFXContext * ctx)
{
	load_Audio(&ctx->effects.break_block, "resources/sfx/break_block16.wav",
			   false);
	load_Audio(&ctx->effects.place_block, "resources/sfx/place_block16.wav",
			   false);
}

void quit_SFX(SFXContext * ctx)
{
	alutExit();
	delete_Audio(&ctx->effects.break_block);
	delete_Audio(&ctx->effects.place_block);
}
