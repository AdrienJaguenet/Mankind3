SRC=main.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Werror -Wextra -Wall
LDFLAGS=`pkg-config --libs sdl2`
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
