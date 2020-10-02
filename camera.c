#include "camera.h"
#include <math.h>

vec3_t get_Camera_forward(Camera * camera)
{
	return v3_norm(vec3(sinf(camera->rotation.x), 0, cosf(camera->rotation.y))
	  );
}

vec3_t get_Camera_lookAt(Camera * camera)
{
	return v3_norm(vec3(sinf(camera->rotation.x) * cosf(camera->rotation.y),
						sinf(camera->rotation.y),
						cosf(camera->rotation.x) * cosf(camera->rotation.y)
				   ));
}

vec3_t get_Camera_right(Camera * camera)
{
	return v3_cross(get_Camera_lookAt(camera), get_Camera_up(camera));
}

vec3_t get_Camera_up(Camera * camera)
{
	(void) camera;
	return vec3(0., 1., 0.);
}

void setup_camera(program_t * program, SDL_Window * window, Camera * camera)
{
	int view_height = 800, view_width = 600;
	SDL_GetWindowSize(window, &view_width, &view_height);
	mat4_t view = m4_look_at(camera->position,
							 v3_add(camera->position,
									get_Camera_lookAt(camera)),
							 get_Camera_up(camera)), projection =
	  m4_perspective(90, 1.0f * view_width / view_height, 0.01f, 1000.0f);

	glUniformMatrix4fv(glGetUniformLocation(program->id, "view"), 1, GL_FALSE,
					   (float *) &view);
	glUniformMatrix4fv(glGetUniformLocation(program->id, "projection"), 1,
					   GL_FALSE, (float *) &projection);
}
