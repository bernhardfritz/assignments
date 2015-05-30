#include <string.h>

#include "Mesh.h"
#include "List.h"
#include "Vector.h"

void computeFaceNormal(face* f) {
  f->normal = createVector(0.0f, 0.0f, 0.0f);
  vector* v1sv0 = createVector(0.0f, 0.0f, 0.0f);
  vector* v2sv0 = createVector(0.0f, 0.0f, 0.0f);
  SubtractVector(&((vertex*)list_get_index(&f->vertexlist, 1))->position, &((vertex*)list_get_index(&f->vertexlist, 0))->position, v1sv0);
  SubtractVector(&((vertex*)list_get_index(&f->vertexlist, 2))->position, &((vertex*)list_get_index(&f->vertexlist, 0))->position, v2sv0);
  MultiplyVector(v1sv0, v2sv0, f->normal);
  SetUnitVector(f->normal);
  destroyVector(v1sv0);
  destroyVector(v2sv0);
}

void computeVertexNormal(vertex* v) {
  for(int i=0; i < v->facelist.item_count; i++) {
    face* f = (face*) list_get_index(&v->facelist, i);
    AddVector(&v->normal, f->normal, &v->normal);
  }
  SetUnitVector(&v->normal);
}

mesh* createMesh(GLfloat* vertex_buffer_data, GLushort* index_buffer_data, GLfloat* color_buffer_data, GLfloat* normal_buffer_data, int facecount, int vertexcount) {
  mesh* result = malloc(sizeof(mesh));
  result->facecount = facecount;
  result->vertexcount = vertexcount;
  result->faces = malloc(facecount*sizeof(face));
  result->vertices = malloc(vertexcount*sizeof(vertex));

  for(int i=0; i<vertexcount; i++) {
    vertex* v = &result->vertices[i];
    v->position.x = &vertex_buffer_data[i*3+0];
    v->position.y = &vertex_buffer_data[i*3+1];
    v->position.z = &vertex_buffer_data[i*3+2];
  }

  for(int i=0; i<facecount; i++) {
    face* f = &result->faces[i];
    list_make(&f->vertexlist,3,0);
    list_add_item(&f->vertexlist, &result->vertices[index_buffer_data[i*3+0]], NULL);
    list_add_item(&f->vertexlist, &result->vertices[index_buffer_data[i*3+1]], NULL);
    list_add_item(&f->vertexlist, &result->vertices[index_buffer_data[i*3+2]], NULL);
    computeFaceNormal(f);
  }

  for(int i=0; i < vertexcount; i++) { // Associate vertices with their faces
    vertex* v = &result->vertices[i];
    list_make(&v->facelist,6,1);
    for(int j=0; j < result->facecount; j++) {
      face* f = &result->faces[j];
      if((vertex*) list_get_item(&f->vertexlist, v) != NULL) list_add_item(&v->facelist, f, NULL);
    }
    v->normal.x = &normal_buffer_data[i*3+0];
    v->normal.y = &normal_buffer_data[i*3+1];
    v->normal.z = &normal_buffer_data[i*3+2];
    computeVertexNormal(v);
  }
  return result;
}

void destroyMesh(mesh* m) {
  for(int i=0; i < m->facecount; i++) {
    face* f = &m->faces[i];
    free(f->normal);
    list_free(&f->vertexlist);
  }
  for(int i=0; i< m->vertexcount; i++) {
    vertex* v = &m->vertices[i];
    list_free(&v->facelist);
  }
  free(m->faces);
  free(m->vertices);
  free(m);
}
