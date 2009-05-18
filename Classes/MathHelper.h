/*
 *  Math.h
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/13/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_

#include <math.h>

typedef struct Vector {
	float_t x,
			y,
			z;
} Vector;


Vector vector_add(const Vector *v, const Vector *u);
Vector vector_cross(const Vector *v, const Vector *u);
float_t vector_dot(const Vector *v, const Vector *u);
float_t vector_length(const Vector *v);
Vector vector_lerp(const Vector *v, const Vector *u, float_t t);
Vector vector_mul(const Vector *v, float_t scale);
Vector vector_normalize(const Vector *v);
Vector vector_slerp(const Vector *v, const Vector *u, float_t t);
Vector vector_sub(const Vector *v, const Vector *u);

#endif /* !_MATHHELPER_H_ */
