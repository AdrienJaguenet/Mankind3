#include "texture.h"
#include "utilities.h"
#include <SDL2/SDL_image.h>

bool load_texture(Texture * texture, const char *path)
{
	SDL_Surface *surface = IMG_Load(path);
	if (surface == NULL) {
		INFO("Could not load texture from path '%s'\n", path);
		return false;
	}
	glGenTextures(1, &texture->index);
	glBindTexture(GL_TEXTURE_2D, texture->index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(surface);
	return true;
}

