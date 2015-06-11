#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct {
  float* x;
  float* y;
  float* z;
} vector;

vector* createVector(float x, float y, float z);
void destroyVector(vector* v);
void SetNullVector(vector* result);
float GetVectorLength(vector* v);
void ScaleVector(float factor, vector* result);
void SetUnitVector(vector* result);
void AddVector(vector* v1, vector* v2, vector* result);
void CopyVector(vector* v1, vector* result);
void SubtractVector(vector* v1, vector* v2, vector* result);
void MultiplyVector(vector* v1, vector* v2, vector* result);
void TranslateVector(float x, float y, float z, vector* result);

#endif // __VECTOR_H__
