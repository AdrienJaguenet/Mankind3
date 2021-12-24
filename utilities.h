#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <stdbool.h>
#include <string.h>

#define COLOR_BLUE     "\x1B[34m"
#define COLOR_RED      "\x1B[31m"
#define COLOR_YELLOW   "\x1B[33m"
#define COLOR_RESET    "\x1B[0m"

#define INFO(fmt, args...) (fprintf (stderr, COLOR_BLUE "%s:%d:%s(): " COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args))
#define WARN(fmt, args...) (fprintf (stderr, COLOR_YELLOW "%s:%d:%s(): " COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args))
#define FATAL(fmt, args...) (fprintf (stderr, COLOR_RED "%s:%d:%s(): " COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args), exit (-1))

#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

typedef struct vec2_t {
	float x, y;
} vec2_t;

static inline vec2_t vec2(float x, float y)
{
	return (vec2_t) {
	x, y};
}

#define PRINT_STACK() do{\
	int PRINT_STACK_addresses_no;\
	void *PRINT_STACK_buffer[50];\
	char **PRINT_STACK_symbols = NULL;\
	PRINT_STACK_addresses_no = backtrace(PRINT_STACK_buffer, 50);\
	PRINT_STACK_symbols = backtrace_symbols(PRINT_STACK_buffer, PRINT_STACK_addresses_no);\
	for (int i = 0; i < PRINT_STACK_addresses_no; ++i) {\
		char *PRINT_STACK_fname_start = strchr(PRINT_STACK_symbols[i], '(') + 1;\
		char *PRINT_STACK_fname_stop = strchr(PRINT_STACK_symbols[i], '+');\
		if (PRINT_STACK_fname_start == NULL || PRINT_STACK_fname_stop == NULL) {\
			continue;\
		}\
		int PRINT_STACK_fname_length = PRINT_STACK_fname_stop - PRINT_STACK_fname_start;\
		if (PRINT_STACK_fname_start) { \
			INFO("\tin:\t%.*s()", PRINT_STACK_fname_length, PRINT_STACK_fname_start);\
		}\
	}\
} while(0)

#define DEBUG_GL() do {\
int DEBUG_GL_error = 0;\
bool DEBUG_GL_had_error = false;\
while ((DEBUG_GL_error = glGetError())) {\
	DEBUG_GL_had_error = true;\
	switch(DEBUG_GL_error) {\
		case GL_NO_ERROR:\
			INFO("No GL error");\
			break;\
		case GL_INVALID_ENUM:\
			INFO("Invalid enum");\
			break;\
		case GL_INVALID_OPERATION:\
			INFO("Invalid operation");\
			break;\
		case GL_INVALID_VALUE:\
			INFO("Invalid value");\
			break;\
		case GL_INVALID_FRAMEBUFFER_OPERATION:\
			INFO("Invalid framebuffer operation");\
			break;\
		case GL_OUT_OF_MEMORY:\
			INFO("Out of memory");\
			break;\
		case GL_STACK_UNDERFLOW:\
			INFO("Stack underflow");\
			break;\
		case GL_STACK_OVERFLOW:\
			INFO("Stack overflow");\
			break;\
	}\
}\
if (DEBUG_GL_had_error) { \
	PRINT_STACK();\
}\
} while(0)
