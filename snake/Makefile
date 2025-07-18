# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 -std=c11
LDFLAGS = -lncursesw

# Source files
SRC = ../cargparse/cargparse.c snake.c snakelib.c snakerendering.c
OBJ = $(SRC:.c=.o)

# Target executable
TARGET = snake

# Default target
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# This includes cargparse.o
../cargparse/cargparse.o: ../cargparse/cargparse.c ../cargparse/cargparse.h
	$(CC) $(CFLAGS) -I../cargparse -c $< -o $@

# Compile each .c file to .o, tracking header dependencies
%.o: %.c snake.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ)