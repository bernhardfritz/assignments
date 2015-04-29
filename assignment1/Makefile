CC = gcc
OBJ = MerryGoRound.o LoadShader.o Matrix.o ObjLoader.o
CFLAGS = -g -Wall -Wextra

OS := $(shell uname)
ifeq ($(OS),Darwin)
# MacOSX
LDLIBS=-framework GLUT -framework OpenGL
else
# Linux
LDLIBS=-lm -lglut -lGLEW -lGL
endif

MerryGoRound: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f *.o MerryGoRound
.PHONY: all clean
