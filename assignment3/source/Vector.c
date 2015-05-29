#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Vector.h"

vector* createVector(float x, float y, float z) {
  vector* result = malloc(sizeof(vector));
  result->x = malloc(sizeof(float));
  result->y = malloc(sizeof(float));
  result->z = malloc(sizeof(float));
  *result->x = x;
  *result->y = y;
  *result->z = z;
  return result;
}

void destroyVector(vector* v) {
  free(v->x);
  free(v->y);
  free(v->z);
  free(v);
}

void SetNullVector(vector* result) {
    *result->x = 0.0f;
    *result->y = 0.0f;
    *result->z = 0.0f;
}

float GetVectorLength(vector* v) {
  return sqrtf(powf(*v->x, 2) + powf(*v->y, 2) + powf(*v->z, 2));
}

void ScaleVector(float factor, vector* result) {
  *result->x *= factor;
  *result->y *= factor;
  *result->z *= factor;
}

void SetUnitVector(vector* result) {
  float length = GetVectorLength(result);
  ScaleVector(1.0f/length, result);
}

void AddVector(vector* v1, vector* v2, vector* result) {
  *result->x = *v1->x + *v2->x;
  *result->y = *v1->y + *v2->y;
  *result->z = *v1->z + *v2->z;
}

vector* CloneVector(vector* v) {
  return createVector(*v->x, *v->y, *v->z);
}

void SubtractVector(vector* v1, vector* v2, vector* result) {
  vector* temp = CloneVector(v2);
  *temp->x *= -1.0f;
  *temp->y *= -1.0f;
  *temp->z *= -1.0f;
  AddVector(v1, temp, result);
  destroyVector(temp);
}

void MultiplyVector(vector* v1, vector* v2, vector* result) {
  *result->x = (*v1->y) * (*v2->z) - (*v1->z) * (*v2->y);
  *result->y = (*v1->z) * (*v2->x) - (*v1->x) * (*v2->z);
  *result->z = (*v1->x) * (*v2->y) - (*v1->y) * (*v2->x);
}

void TranslateVector(float x, float y, float z, vector* result) {
  vector* temp = createVector(x,z,y);
  AddVector(result, temp, result);
  destroyVector(temp);
}
