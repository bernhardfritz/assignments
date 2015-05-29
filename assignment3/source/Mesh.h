#ifndef __MESH_H__
#define __MESH_H__

/* OpenGL includes */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include "Vector.h"

typedef struct {
  float r;
  float g;
  float b;
} color;

typedef struct {
  vector position;
  vector normal;
  color c;
} vertex;

typedef struct {
  vertex vertices[3];
  vector* normal;
} face;

typedef struct {
  face* faces;
  int facecount;
  int vertexcount;
} mesh;

void computeFaceNormal(face* f);
void computePerVertexNormals(mesh* m);
mesh* createMesh(GLfloat* vertex_buffer_data, GLushort* index_buffer_data, GLfloat* color_buffer_data, int facecount, int vertexcount);
void destroyMesh(mesh* m);

#endif // __MATRIX_H__
