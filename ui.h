#pragma once

#include "program.h"
#include "texture.h"
#include <SDL2/SDL_ttf.h>

typedef enum {
	POSITION_ABSOLUTE = 0,
	POSITION_RELATIVE = 1
} EPosition;

typedef enum {
	DISPLAY_NONE = 0,
	DISPLAY_INLINE = 1,
	DISPLAY_BLOCK = 2
} EDisplay;

typedef struct UI UI;

struct UIElement {
	GLint top_px, left_px, height_px, width_px;
	EPosition position;
	EDisplay display;
	bool centered;
	int children_no;
	struct UIElement **children;
	struct UIElement *parent;
	Texture texture;
	UI *ui;
};

typedef struct UIElement UIElement;

struct UI {
	UIElement *root;
	TTF_Font * main_font;
	program_t program;
	int width_px;
	int height_px;
	/* Quad VAO, this is shared by all UI elements */
	GLuint vao;
};

UIElement *UIElement_new_image(UI * ui, const char *path);

UIElement *UIElement_load(UI * ui);

void UIElement_delete(UIElement * element);

void UIElement_draw(UIElement * element);

void UIElement_addChild(UIElement * parent, UIElement * child);

void load_UI(UI * ui);

void unload_UI();

void UIElement_center(UIElement * element);
