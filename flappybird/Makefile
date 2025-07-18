# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 -std=c11
LDFLAGS = -lrt -lm -lncursesw

# Source files
SRC = ../cargparse/cargparse.c flappybird.c flappybirdlib.c flappybirdrendering.c
OBJ = $(SRC:.c=.o)

# Target executable
TARGET = flappybird

# Default target
all: $(TARGET)

cargparse.o: ../cargparse/cargparse.c cargparse.h
	$(CC) $(CFLAGS) -c $< -I../cargparse -o $@

# Compile each .c file to .o, tracking header dependencies
%.o: %.c flappybird.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into final binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(OBJ)