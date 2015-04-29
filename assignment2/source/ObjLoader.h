#ifndef __OBJLOADER_H__
#define __OBJLOADER_H__

void loadObj(GLfloat* result_vertex_buffer_data, GLushort* result_index_buffer_data, GLfloat* result_color_buffer_data, int vertices, int faces, char* fname);
int count(char x, char* fname);

#endif // __OBJLOADER_H__
