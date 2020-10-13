#include "soundfx.h"

void load_Audio(Audio * audio, const char *path)
{
	audio->buffer = alutCreateBufferFromFile(path);

	if (audio->buffer == AL_NONE) {
		FATAL("ALUT error: %s", alutGetErrorString(alutGetError()));
	}

	/* alSourcef(audio->source, AL_REFERENCE_DISTANCE, 0); */
	/* alSourcef(audio->source, AL_MAX_DISTANCE, 1); */
}

void play_Audio(SFXContext * ctx, Audio * audio, bool looping)
{
	for (int i = 0; i < NUM_SOURCES; ++i) {
		ALenum state;
		alGetSourcei(ctx->sources[i], AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING) {
			alSourcei(ctx->sources[i], AL_LOOPING, looping);
			alSourcei(ctx->sources[i], AL_BUFFER, audio->buffer);
			alSourcePlay(ctx->sources[i]);
			break;
		}

		if (i == NUM_SOURCES - 1) {
			INFO
			  ("Ran out of OpenAL sources, increase NUM_SOURCES in soundfx.h.");
		}
	}
}

void play_Audio_at(SFXContext * ctx, Audio * audio, vec3_t * position)
{
	for (int i = 0; i < NUM_SOURCES; ++i) {
		ALenum state;
		alGetSourcei(ctx->sources[i], AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING) {
			alSourcei(ctx->sources[i], AL_BUFFER, audio->buffer);
			alSource3f(ctx->sources[i], AL_POSITION, position->x, position->y,
					   position->z);
			alSourcePlay(ctx->sources[i]);
			break;
		}

		if (i == NUM_SOURCES - 1) {
			INFO
			  ("Ran out of OpenAL sources, increase NUM_SOURCES in soundfx.h.");
		}
	}
}

void delete_Audio(Audio * audio)
{
	alDeleteBuffers(1, &audio->buffer);
}

void init_SFX(SFXContext * ctx)
{
	alutInit(0, NULL);
	alGetError();

	ctx->sources = malloc(sizeof(ALuint) * NUM_SOURCES);

	for (int i = 0; i < NUM_SOURCES; ++i) {
		alGenSources(1, &ctx->sources[i]);
	}

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
	load_Audio(&ctx->effects.break_block, "resources/sfx/break_block16.wav");
	load_Audio(&ctx->effects.place_block, "resources/sfx/place_block16.wav");
	load_Audio(&ctx->music.ambient, "resources/sfx/ambient16.wav");
}

void quit_SFX(SFXContext * ctx)
{
	for (int i = 0; i < NUM_SOURCES; ++i) {
		alDeleteSources(1, &ctx->sources[i]);
	}

	delete_Audio(&ctx->effects.break_block);
	delete_Audio(&ctx->effects.place_block);
	delete_Audio(&ctx->music.ambient);
	alutExit();
}
