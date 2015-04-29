CC = gcc
OBJ = objloaderelephant.o
CFLAGS = -g -Wall -Wextra

OS := $(shell uname)
ifeq ($(OS),Darwin)
# MacOSX
LDLIBS=-framework GLUT -framework OpenGL
else
# Linux
LDLIBS=-lm -lglut -lGLEW -lGL
endif

objloaderelephant: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f *.o objloaderelephant
.PHONY: all clean
