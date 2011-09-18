#pragma once

#include <cmath>

namespace MyMath
{
	inline void normalize(float& x, float& y, float& z);

	inline void vec_normalize(float* v);

	inline float vec_len(float x, float y, float z);

	inline float vec_len(const float* v);
	inline float vec_len_square(const float* v);

	inline void vec_subs(const float* v1, const float* v2, float* result);

	inline void vec_add_scaled(float* v1, const float* v2, float scale);

	inline void vec_divide(float* v, float divident);

	inline void vec_cross(const float* v1, const float* v2, float* result);

	inline float vec_dot(const float* v1, const float* v2);

	inline float vec_cosinus(const float* v1, const float* v2);

	inline float vec_angle(const float* v1, const float* v2);

	inline void vec_create(float* result, float x1, float y1, float z1, float x2, float y2, float z2);

	inline void vec_scale(float* vec, float scale);
}; // namespace MyMath


void MyMath::normalize(float& x, float& y, float& z)
{
	float len = sqrt(x*x + y*y + z*z);
	x /= len;
	y /= len;
	z /= len;
}

void MyMath::vec_normalize(float* v)
{
	vec_divide(v, vec_len(v));
}

void MyMath::vec_divide(float* v, float divident)
{
	v[0] /= divident;
	v[1] /= divident;
	v[2] /= divident;
}

float MyMath::vec_len(float x, float y, float z)
{
	return sqrt(x*x + y*y + z*z);
}

float MyMath::vec_len_square(const float* v)
{
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

float MyMath::vec_len(const float* v)
{
	return sqrt(vec_len_square(v));
}

void MyMath::vec_subs(const float* v1, const float* v2, float* result) 
{
	result[0] = v1[0] - v2[0];
	result[1] = v1[1] - v2[1];
	result[2] = v1[2] - v2[2];
}

void MyMath::vec_cross(const float* v1, const float* v2, float* result)
{
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] =	v1[2] * v2[0] - v1[0] * v2[2];
	result[2] =	v1[0] * v2[1] - v1[1] * v2[0];
}

float MyMath::vec_dot(const float* v1, const float* v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]* v2[2];
}

float MyMath::vec_cosinus(const float* v1, const float* v2)
{
	return vec_dot(v1, v2) / (vec_len(v1) * vec_len(v2));
}

float MyMath::vec_angle(const float* v1, const float* v2)
{
	return acos(vec_cosinus(v1, v2));
}

void MyMath::vec_create(float* result, float x1, float y1, float z1, float x2, float y2, float z2)
{
	result[0] = x1 - x2;
	result[1] = y1 - y2;
	result[2] = z1 - z2;
}

void MyMath::vec_scale(float* vec, float scale)
{
	vec[0] *= scale;
	vec[1] *= scale;
	vec[2] *= scale;
}

void MyMath::vec_add_scaled(float* v1, const float* v2, float scale)
{
	v1[0] += scale * v2[0];
	v1[1] += scale * v2[1];
	v1[2] += scale * v2[2];
}