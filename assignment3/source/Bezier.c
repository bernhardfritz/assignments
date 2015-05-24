void lerp(float* dest, float* a, float* b, float t) {
  dest[0] = a[0] + (b[0] - a[0]) * t;
  dest[1] = a[1] + (b[1] - a[1]) * t;
  dest[2] = a[2] + (b[2] - a[2]) * t;
}

void quadratic_bezier(float* dest, float* a, float* b, float* c, float t) {
  float ab[3];
  float bc[3];
  lerp(ab, a, b, t); // point between a and b
  lerp(bc, b, c, t); // point between b and c
  lerp(dest, ab, bc, t); // point between ab and bc
}
