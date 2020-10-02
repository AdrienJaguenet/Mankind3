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
	if (surface->w != surface->h) {
		WARN("Texture '%s' is not a square\n", path);
	}
	texture->size = surface->w;
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

void get_tex_quad(Texture * texture, int index, int quad_size,
				  vec2_t corners[4])
{
	int tex_grid_size = texture->size / quad_size;
	float cell_size = 1 / (float) tex_grid_size;
	int quad_y = index / tex_grid_size, quad_x = index % tex_grid_size;
	corners[UV_TOP_LEFT] = vec2(quad_x * cell_size, quad_y * cell_size);
	corners[UV_TOP_RIGHT] = vec2((quad_x + 1) * cell_size, quad_y * cell_size);
	corners[UV_BOTTOM_LEFT] =
	  vec2(quad_x * cell_size, (quad_y + 1) * cell_size);
	corners[UV_BOTTOM_RIGHT] =
	  vec2((quad_x + 1) * cell_size, (quad_y + 1) * cell_size);
}
