CC = gcc
LD = gcc

OBJ = Texturing.o LoadShader.o LoadTexture.o
TARGET = Texturing

CFLAGS = -std=c99 -g -Wall -Wextra
OS := $(shell uname)
ifeq ($(OS),Darwin)
# MacOSX
LDLIBS=-lm -framework GLUT -framework OpenGL
else
# Linux
LDLIBS=-lm -lglut -lGLEW -lGL
endif
INCLUDES = -Isource

SRC_DIR = source
BUILD_DIR = build
VPATH = source

# Rules
all: $(TARGET)

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	rm -f $(BUILD_DIR)/*.o *.o $(TARGET)

.PHONY: clean

# Dependencies
$(TARGET): $(BUILD_DIR)/LoadShader.o $(BUILD_DIR)/LoadTexture.o  | $(BUILD_DIR)
