#include "ui.h"
#include <assert.h>

UIElement *UIElement_new(UI * ui, const char *texture_path)
{
	INFO("Creating UIElement");
	UIElement *element = malloc(sizeof(UIElement));
	element->position = POSITION_ABSOLUTE;
	element->top_px = 0;
	element->left_px = 0;
	element->width_px = 0;
	element->height_px = 0;
	element->children_no = 0;
	element->children = NULL;
	element->parent = NULL;
	element->ui = ui;

	INFO("UIElement GL initialization");
	load_texture(&element->texture, texture_path);
	const int vertices_no = 6;
	GLuint vbo;
	/* Always send a simple quad to OpenGL. We then use
	   uniform values to calculate the actual vertex positions. */
	GLfloat vertices[] = {
		/*(X,Y) (U,V)
		 */
		0., 0., 0., 0.,
		0., 1., 0., 1.,
		1., 0., 1., 0.,

		1., 1., 1., 1.,
		1., 0., 1., 0.,
		0., 1., 0., 1.
	};
	/* Generate and bind the VAO */
	glGenVertexArrays(1, &element->vao);
	glBindVertexArray(element->vao);
	/* Generate and bind the VBO */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/* Load data into the VBO */
	glBufferData(GL_ARRAY_BUFFER, (2 + 2) * vertices_no * sizeof(GLfloat),
				 vertices, GL_STATIC_DRAW);
	/* First component (2 float values) : X, Y coordinates of the quad */
	glVertexAttribPointer((GLuint) 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
						  0);
	/* Second component (2 float values) : U, V coordinates for texturing the quad */
	glVertexAttribPointer((GLuint) 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
						  (void *) (2 * sizeof(float)));

	return element;
}

void UIElement_draw(UIElement * element)
{
	for (int i = 0; i < element->children_no; ++i) {
		UIElement_draw(element->children[i]);
	}

	UIElement_center(element);

	program_t *program = &element->ui->program;

	/* set texture0 to active */
	glActiveTexture(GL_TEXTURE0);
	/* Activate element's texture */
	glBindTexture(GL_TEXTURE_2D, element->texture.index);
	/* Bind the element's vertex array */
	glBindVertexArray(element->vao);
	/* Enable attributes 0 (x, y) and 1 (u, v) */
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	/* Pass uniform values */
	glUniform1i(glGetUniformLocation(program->id, "screen_width_px"),
				element->ui->width_px);
	glUniform1i(glGetUniformLocation(program->id, "screen_height_px"),
				element->ui->height_px);
	glUniform1i(glGetUniformLocation(program->id, "top_px"), element->top_px);
	glUniform1i(glGetUniformLocation(program->id, "left_px"), element->left_px);
	glUniform1i(glGetUniformLocation(program->id, "width_px"),
				element->width_px);
	glUniform1i(glGetUniformLocation(program->id, "height_px"),
				element->height_px);
	/* draw */
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void UIElement_center(UIElement * element)
{
	UI *ui = element->ui;
	element->top_px = ui->height_px / 2 - element->height_px / 2;
	element->left_px = ui->width_px / 2 - element->width_px / 2;
}

void UIElement_delete(UIElement * element)
{
	for (int i = 0; i < element->children_no; i++) {
		UIElement_delete(element->children[i]);
	}
	if (element->children != NULL) {
		free(element->children);
	}
	free(element);
}

void load_UI(UI * ui)
{
	ui->program = program_new("./resources/shaders/uielement.vs",
							  "./resources/shaders/uielement.fs");
}

void unload_UI()
{
}
