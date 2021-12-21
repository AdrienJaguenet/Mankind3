#pragma once

#include "program.h"

typedef struct {
    program_t program;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} CubeHighlight ;

void CubeHighlight_init(CubeHighlight * highlight);
