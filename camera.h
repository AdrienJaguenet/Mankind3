#pragma once

#include "math_3d.h"
#include "program.h"
#include <SDL2/SDL.h>

typedef struct {
	vec3_t position;
	vec3_t rotation;
} Camera;

vec3_t get_Camera_forward(Camera * camera);
vec3_t get_Camera_lookAt(Camera * camera);
vec3_t get_Camera_right(Camera * camera);
vec3_t get_Camera_up(Camera * camera);

void setup_camera(program_t * program, SDL_Window * window, Camera * camera);
