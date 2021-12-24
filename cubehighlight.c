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
	highlight->program = program_new("resources/shaders/highlight.vs", "resources/shaders/highlight.fs");
    GLfloat cube_vertices[] = {
    //    x    y    z
        -0.001f, -0.001f, -0.001f, // 0
        -0.001f, -0.001f,  1.001f, // 1
        -0.001f,  1.001f, -0.001f, // 2
        -0.001f,  1.001f,  1.001f, // 3
         1.001f, -0.001f, -0.001f, // 4
         1.001f, -0.001f,  1.001f, // 5
         1.001f,  1.001f, -0.001f, // 6
         1.001f,  1.001f,  1.001f  // 7
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
	/* We use elements instead of repeating vertices for this one, since we do not
	need light calculations from normals or anything. */

	/* Generate the buffers */
    glGenVertexArrays(1, &highlight->vao);
    glGenBuffers(1, &highlight->vbo);
    glGenBuffers(1, &highlight->ebo);

	// bind the VAO
    glBindVertexArray(highlight->vao);
	
	// bind the vertex buffer and fill it with vertex data
    glBindBuffer(GL_ARRAY_BUFFER, highlight->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	// copy the indices into the ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, highlight->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
