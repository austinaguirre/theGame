CC = gcc
# Include headers from the `include` directory and use `sdl2-config` for SDL2 flags
CFLAGS = -Wall -Iinclude $(shell sdl2-config --cflags)
# Link against SDL2 and SDL2_image, using `sdl2-config` for SDL2 libraries
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image
# Automatically find all C source files in the `src` directory
SRCS = $(wildcard src/*.c)
# Convert the .c filenames to .o filenames for object files
OBJS = $(SRCS:.c=.o)
# The final executable target
TARGET = game

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile each C source file into an object file
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Clean up by removing the executable and object files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
