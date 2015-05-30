#include "Vector.h"

void lerp(vector* dest, vector* a, vector* b, float t) {
  *dest->x = *a->x + (*b->x - *a->x) * t;
  *dest->y = *a->y + (*b->y - *a->y) * t;
  *dest->z = *a->z + (*b->z - *a->z) * t;
}

void quadratic_bezier(vector* dest, vector* a, vector* b, vector* c, float t) {
  vector* ab = createVector(0.0f, 0.0f, 0.0f);
  vector* bc = createVector(0.0f, 0.0f, 0.0f);
  lerp(ab, a, b, t); // point between a and b
  lerp(bc, b, c, t); // point between b and c
  lerp(dest, ab, bc, t); // point between ab and bc
  destroyVector(ab);
  destroyVector(bc);
}
