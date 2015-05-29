#include "Mesh.h"

void computeFaceNormal(face* f) {
  f->normal = createVector(0.0f, 0.0f, 0.0f);
  vector* v1sv0 = createVector(0.0f, 0.0f, 0.0f);
  vector* v2sv0 = createVector(0.0f, 0.0f, 0.0f);
  SubtractVector(&f->vertices[1].position, &f->vertices[0].position, v1sv0);
  SubtractVector(&f->vertices[2].position, &f->vertices[0].position, v2sv0);
  MultiplyVector(v1sv0, v2sv0, f->normal);
  SetUnitVector(f->normal);
  destroyVector(v1sv0);
  destroyVector(v2sv0);
}

void computePerVertexNormals(mesh* m) {
  vertex allVertices[m->vertexcount];
  for(int i=0; i<m->facecount; i++) {
  }
}

mesh* createMesh(GLfloat* vertex_buffer_data, GLushort* index_buffer_data, GLfloat* color_buffer_data, int facecount, int vertexcount) {
  mesh* result = malloc(sizeof(mesh));
  result->facecount = facecount;
  result->vertexcount = vertexcount;
  result->faces = malloc(facecount*sizeof(face));
  for(int i=0; i<facecount; i++) {
    result->faces[i].vertices[0].position.x = &vertex_buffer_data[index_buffer_data[i*3+0]+0];
    result->faces[i].vertices[0].position.y = &vertex_buffer_data[index_buffer_data[i*3+0]+1];
    result->faces[i].vertices[0].position.z = &vertex_buffer_data[index_buffer_data[i*3+0]+2];
    result->faces[i].vertices[1].position.x = &vertex_buffer_data[index_buffer_data[i*3+1]+0];
    result->faces[i].vertices[1].position.y = &vertex_buffer_data[index_buffer_data[i*3+1]+1];
    result->faces[i].vertices[1].position.z = &vertex_buffer_data[index_buffer_data[i*3+1]+2];
    result->faces[i].vertices[2].position.x = &vertex_buffer_data[index_buffer_data[i*3+2]+0];
    result->faces[i].vertices[2].position.y = &vertex_buffer_data[index_buffer_data[i*3+2]+1];
    result->faces[i].vertices[2].position.z = &vertex_buffer_data[index_buffer_data[i*3+2]+2];
    computeFaceNormal(&result->faces[i]);
  }
  //computePerVertexNormals(result.faces);
  return result;
}

void destroyMesh(mesh* m) {
  for(int i=0; i < m->facecount; i++) {
    free(m->faces[i].normal);
  }
  free(m->faces);
  free(m);
}
