#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void SetNullVector(float* result) {
    for(int i=0; i<3; i++) {
      result[i]=0.0;
    }
}

float GetVectorLength(float* vector) {
  return sqrtf(powf(vector[0],2) + powf(vector[1],2) + powf(vector[2],2));
}

void ScaleVector(float factor, float* result) {
  for(int i=0; i<3; i++) {
    result[i] *= factor;
  }
}

void SetUnitVector(float* result) {
  float length = GetVectorLength(result);
  ScaleVector(1.0/length, result);
}

void AddVector(float* vector1, float* vector2, float* result) {
  for(int i=0; i<3; i++) {
    result[i] = vector1[i] + vector2[i];
  }
}

void CopyVector(float* vector1, float* result) {
  memcpy(result, vector1, 3*sizeof(float));
}

void SubtractVector(float* vector1, float* vector2, float* result) {
  float temp[3];
  CopyVector(vector2, temp);
  for(int i=0; i<3; i++) {
    temp[i] *= -1.0;
  }
  AddVector(vector1, temp, result);
}

void MultiplyVector(float* vector1, float* vector2, float* result) {
  result[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
  result[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
  result[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
}

void TranslateVector(float x, float y, float z, float* result) {
  float temp[3] = {x,y,z};
  AddVector(result,temp,result);
}
