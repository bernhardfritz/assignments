/* OpenGL includes */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void loadObj(GLfloat* result_vertex_buffer_data, GLushort* result_index_buffer_data, GLfloat* result_color_buffer_data, int vertices, int faces, char* fname) {
  FILE* fp;
  int read;
  GLfloat x, y, z;
  char ch;
  fp=fopen(fname,"r");

  if (!fp){
    printf("can't open file %s\n", fname);
	  exit(1);
  }

  int vertex_buffer_size = vertices*3;
  int color_buffer_size = vertex_buffer_size;
  int index_buffer_size = faces*3;
  GLfloat temp_vertex_buffer_data[vertex_buffer_size];
  GLfloat temp_color_buffer_data[color_buffer_size];
  GLushort temp_index_buffer_data[index_buffer_size];
  int vertex_buffer_counter = 0;
  int color_buffer_counter = 0;
  int index_buffer_counter = 0;

  srand((unsigned int)time(NULL));

  while(!(feof(fp))) {
    read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
    if(read==4&&ch=='v') {
      temp_vertex_buffer_data[vertex_buffer_counter++] = x;
      temp_vertex_buffer_data[vertex_buffer_counter++] = y;
      temp_vertex_buffer_data[vertex_buffer_counter++] = z;
      temp_color_buffer_data[color_buffer_counter++] = (float)rand()/(float)(RAND_MAX);
      temp_color_buffer_data[color_buffer_counter++] = (float)rand()/(float)(RAND_MAX);
      temp_color_buffer_data[color_buffer_counter++] = (float)rand()/(float)(RAND_MAX);
    } else if(read==4&&ch=='f') {
      temp_index_buffer_data[index_buffer_counter++] = x-1;
      temp_index_buffer_data[index_buffer_counter++] = y-1;
      temp_index_buffer_data[index_buffer_counter++] = z-1;
    }
  }

  fclose(fp);
  memcpy(result_vertex_buffer_data, temp_vertex_buffer_data, vertex_buffer_size*sizeof(GLfloat));
  memcpy(result_color_buffer_data, temp_color_buffer_data, color_buffer_size*sizeof(GLfloat));
  memcpy(result_index_buffer_data, temp_index_buffer_data, index_buffer_size*sizeof(GLushort));
}

int count(int i, char* fname) {
  /* a buffer to hold the count of characters 0,...,256; it is
   * initialized to zero on every element */
  int count[256] = { 0 };

  /* file handle --- in this case I am parsing this source code */
  FILE *fp = fopen(fname, "r");

  /* a holder for each character (stored as int) */
  int c;

  /* for as long as we can get characters... */
  while((c=fgetc(fp))) {

    /* break if end of file */
    if(c == EOF) break;

    /* otherwise add one to the count of that particular character */
    count[c]+=1;
  }

  /* close the file */
  fclose(fp);

  /* that's it */
  return count[i];
}
