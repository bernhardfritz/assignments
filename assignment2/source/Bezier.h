#ifndef __BEZIER_H__
#define __BEZIER_H__

void lerp(float* dest, float* a, float* b, float t);
void quadratic_bezier(float* dest, float* a, float* b, float* c, float t);

#endif // __BEZIER_H__
