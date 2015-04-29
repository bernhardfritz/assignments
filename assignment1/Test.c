/* OpenGL includes */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "ObjLoader.h"

int main(void) {
  char* fname="data/teddy.obj";
  int vertices = count('v',fname);
  int faces = count('f',fname);
  printf("%d %d",vertices,faces);
  GLfloat vertex_buffer_data[vertices*3];
  GLfloat color_buffer_data[vertices*3];
  GLushort index_buffer_data[faces*3];
  loadObj(vertex_buffer_data, color_buffer_data, index_buffer_data, vertices, faces, fname);
}
