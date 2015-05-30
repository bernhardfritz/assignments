#include <string.h>

#include "Mesh.h"
#include "List.h"
#include "Vector.h"

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
  list_make(&result->vertexlist,vertexcount,0);
  for(int i=0; i<facecount; i++) {
    result->faces[i].vertices[0].position.x = &vertex_buffer_data[index_buffer_data[i*3+0]+0];
    result->faces[i].vertices[0].position.y = &vertex_buffer_data[index_buffer_data[i*3+0]+1];
    result->faces[i].vertices[0].position.z = &vertex_buffer_data[index_buffer_data[i*3+0]+2];
    if(list_get_item(&result->vertexlist, &result->faces[i].vertices[0]) == NULL) list_add_item(&result->vertexlist, &result->faces[i].vertices[0], NULL);
    result->faces[i].vertices[1].position.x = &vertex_buffer_data[index_buffer_data[i*3+1]+0];
    result->faces[i].vertices[1].position.y = &vertex_buffer_data[index_buffer_data[i*3+1]+1];
    result->faces[i].vertices[1].position.z = &vertex_buffer_data[index_buffer_data[i*3+1]+2];
    if(list_get_item(&result->vertexlist, &result->faces[i].vertices[1]) == NULL) list_add_item(&result->vertexlist, &result->faces[i].vertices[1], NULL);
    result->faces[i].vertices[2].position.x = &vertex_buffer_data[index_buffer_data[i*3+2]+0];
    result->faces[i].vertices[2].position.y = &vertex_buffer_data[index_buffer_data[i*3+2]+1];
    result->faces[i].vertices[2].position.z = &vertex_buffer_data[index_buffer_data[i*3+2]+2];
    if(list_get_item(&result->vertexlist, &result->faces[i].vertices[2]) == NULL) list_add_item(&result->vertexlist, &result->faces[i].vertices[2], NULL);
    computeFaceNormal(&result->faces[i]);
  }
  for(int i=0; i < result->vertexlist.item_count; i++) { // Associate vertices with their faces
    vertex* v = (vertex*) list_get_index(&result->vertexlist, i);
    list_make(&v->facelist,4,1);
    for(int j=0; j < result->facecount; j++) {
      face* f = &result->faces[j];
      if((vertex*) list_get_item(&result->vertexlist, &result->faces[i].vertices[0]) == v) list_add_item(&v->facelist, f, NULL);
      if((vertex*) list_get_item(&result->vertexlist, &result->faces[i].vertices[1]) == v) list_add_item(&v->facelist, f, NULL);
      if((vertex*) list_get_item(&result->vertexlist, &result->faces[i].vertices[2]) == v) list_add_item(&v->facelist, f, NULL);
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
    free(m->faces[i].normal);
  }
  for(int i=0; i < m->vertexlist.item_count; i++) {
    vertex* v = (vertex*) list_get_index(&m->vertexlist, i);
    free(v->normal);
  }
  free(m->faces);
  free(m);
}
