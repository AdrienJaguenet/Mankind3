#pragma once

#include "program.h"
#include "texture.h"

typedef enum {
	POSITION_ABSOLUTE = 0,
	POSITION_RELATIVE = 1
} EPosition;

typedef struct UI UI;

struct UIElement {
	float top_px, left_px, height_px, width_px;
	EPosition position;
	int children_no;
	struct UIElement **children;
	struct UIElement *parent;
	GLuint vao;
	Texture texture;
  UI * ui;
};

typedef struct UIElement UIElement;

struct UI {
	UIElement *root;
	program_t program;
  int width_px;
  int height_px;
};

UIElement *UIElement_new(UI * ui, const char * texture_path);

void UIElement_delete(UIElement * element);

void UIElement_draw(UIElement * element);

void load_UI(UI * ui);

void unload_UI();
