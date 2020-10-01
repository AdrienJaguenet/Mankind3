#pragma once

typedef struct program_t
{
  GLint program_id;
  GLint vs_id, fs_id;
} program_t;

program_t program_new(const char *vs_path, const char *fs_path);
void program_use();
void program_terminate();
