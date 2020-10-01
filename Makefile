SRC=main.c engine.c map.c position.c mesh.c texture.c program.c
OBJ=$(SRC:.c=.o)
CFLAGS=-g -Werror -Wextra -Wall `pkg-config --cflags sdl2 SDL2_image gl glew`
LDFLAGS=`pkg-config --libs sdl2 SDL2_image gl glew`
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
