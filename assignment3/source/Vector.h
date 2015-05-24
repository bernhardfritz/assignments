#ifndef __VECTOR_H__
#define __VECTOR_H__

void SetNullVector(float* result);
float GetVectorLength(float* vector);
void ScaleVector(float factor, float* result);
void SetUnitVector(float* result);
void AddVector(float* vector1, float* vector2, float* result);
void CopyVector(float* vector1, float* result);
void SubtractVector(float* vector1, float* vector2, float* result);
void MultiplyVector(float* vector1, float* vector2, float* result);
void TranslateVector(float x, float y, float z, float* result);

#endif // __VECTOR_H__
