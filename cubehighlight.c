#include "cubehighlight.h"

/*  y   z
    |  /
    | /
     --- x
 */

/*   3 -- 7
    /|   /|
   2 -- 6 |
   | 1 -| 5
   |/   |/
   0 -- 4
*/

void CubeHighlight_init(CubeHighlight * highlight) {
    GLfloat cube_vertices[] = {
    //    x    y    z
        -0.1f, -0.1f, -0.1f, // 0
        -0.1f, -0.1f,  1.1f, // 1
        -0.1f,  1.1f, -0.1f, // 2
        -0.1f,  1.1f,  1.1f, // 3
         1.1f, -0.1f, -0.1f, // 4
         1.1f, -0.1f,  1.1f, // 5
         1.1f,  1.1f, -0.1f, // 6
         1.1f,  1.1f,  1.1f  // 7
    };
    GLuint indices[] = {
        0, 1,
        0, 2,
        0, 4,
        1, 3,
        1, 5,
        2, 6,
        2, 3,
        3, 7,
        4, 6,
        4, 5,
        5, 7,
        6, 7
    };
    glGenVertexArrays(1, &highlight->vao);
    glBindVertexArray(highlight->vao);
    glGenBuffers(1, &highlight->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, highlight->vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * 8 * sizeof(GLfloat), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &highlight->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, highlight->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 2 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glVertexArrayElementBuffer(highlight->vao, highlight->ebo);
}
