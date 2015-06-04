CC = gcc
LD = gcc

OBJ = MerryGoRound.o LoadShader.o Mesh.o Vector.o Matrix.o Bezier.o ColorConversion.o StringExtra.o OBJParser.o List.o
TARGET = MerryGoRound

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
$(TARGET): $(BUILD_DIR)/LoadShader.o $(BUILD_DIR)/Mesh.o $(BUILD_DIR)/Vector.o $(BUILD_DIR)/Matrix.o $(BUILD_DIR)/Bezier.o $(BUILD_DIR)/ColorConversion.o $(BUILD_DIR)/StringExtra.o $(BUILD_DIR)/OBJParser.o  $(BUILD_DIR)/List.o | $(BUILD_DIR)
