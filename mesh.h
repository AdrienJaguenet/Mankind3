#pragma once

typedef struct mesh_t
{
  GLuint vao, vbo, ebo;
} mesh_t;

mesh_t mesh_new ();
void mesh_render ();
