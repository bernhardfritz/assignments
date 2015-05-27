CC=gcc
CFLAGS=-Wall -I/usr/include
LDFLAGS=-pthread -L/usr/lib -lm -framework GLUT -framework OpenGL
OBJS=main.o scene.o shader.o

glsl_lighting:	$(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o glsl_lighting

clean:
	rm -f glsl_lighting
	rm -f $(OBJS)

main.o: main.c
scene.o: scene.c
shader.o: shader.c
