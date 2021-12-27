SRC=main.c\
	map.c\
	position.c\
	mesh.c\
	texture.c\
	program.c\
	chunkmesh.c\
	camera.c\
	math_3d.c\
	graphx.c\
	heap.c\
	noise.c\
	boxcol.c\
	physx.c\
	raycast.c\
	soundfx.c\
	hashmap.c\
	biome.c\
	ui.c\
	cubehighlight.c\
	movement.c\
	events.c

OBJ=$(SRC:.c=.o)
CFLAGS=-g -Werror -Wextra -Wall `pkg-config --cflags sdl2 SDL2_image openal freealut gl glew`
LDFLAGS=`pkg-config --libs sdl2 SDL2_image openal freealut gl glew` -lm -rdynamic
TARGET=mankind
CC=gcc
LD=gcc

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(LD) $^ -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	$(RM) $(TARGET) $(OBJ)
