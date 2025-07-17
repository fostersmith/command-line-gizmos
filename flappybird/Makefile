# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 -std=c11
LDFLAGS = -lrt -lm -lncursesw

# Source files
SRC = flappybird.c flappybirdlib.c flappybirdrendering.c
OBJ = $(SRC:.c=.o)

# Target executable
TARGET = flappybird

# Default target
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c file to .o, tracking header dependencies
%.o: %.c flappybird.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ)