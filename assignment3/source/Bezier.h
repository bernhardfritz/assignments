#ifndef __BEZIER_H__
#define __BEZIER_H__

#include "Vector.h"

void lerp(vector* dest, vector* a, vector* b, float t);
void quadratic_bezier(vector* dest, vector* a, vector* b, vector* c, float t);

#endif // __BEZIER_H__
