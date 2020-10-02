#pragma once

#define COLOR_BLUE     "\x1B[34m"
#define COLOR_RED      "\x1B[31m"
#define COLOR_YELLOW   "\x1B[33m"
#define COLOR_RESET    "\x1B[0m"

#define INFO(fmt, args...) (fprintf (stderr, COLOR_BLUE "%s:%d:%s(): " COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args))
#define WARN(fmt, args...) (fprintf (stderr, COLOR_YELLOW "%s:%d:%s(): " COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args))
#define FATAL(fmt, args...) (fprintf (stderr, COLOR_RED "%s:%d:%s(): " COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args), exit (-1))

typedef struct vec2_t {
	float x, y;
} vec2_t;

static inline vec2_t vec2(float x, float y)
{
	return (vec2_t) {
	x, y};
}
