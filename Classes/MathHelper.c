/*
 *  Math.c
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/13/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#include <strings.h>

#include "MathHelper.h"

Vector
vector_add(const Vector *v, const Vector *u)
{
	Vector w;
	w.x = v->x + u->x;
	w.y = v->y + u->y;
	w.z = v->z + u->z;
	return w;
}

Vector
vector_cross(const Vector *v, const Vector *u)
{
	/*e1 e2 e3
	 x  y  z
	 x  y  z*/
	
	Vector w;
	w.x = v->y * u->z - v->z * u->y;
	w.y = v->x * u->z - v->z * u->x;
	w.z = v->x * u->y - v->y * u->x;
	return w;
}

float_t
vector_dot(const Vector *v, const Vector *u)
{
	return (v->x * u->x) + (v->y * u->y) + (v->z * u->z);
}

float_t
vector_length(const Vector *v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

Vector
vector_mul(const Vector *v, float_t scale)
{
	Vector w;
	w.x = scale * v->x;
	w.y = scale * v->y;
	w.z = scale * v->z;
	return w;
}

Vector
vector_normalize(const Vector *v)
{
	float_t len = vector_length(v);
	Vector u;
	
	u.x = v->x / len;
	u.y = v->y / len;
	u.z = v->z / len;
	
	return u;
}

Vector
vector_sub(const Vector *v, const Vector *u)
{
	Vector w;
	
	w.x = v->x - u->x;
	w.y = v->y - u->y;
	w.z = v->z - u->z;
	return w;
}

Vector
vector_lerp(const Vector *v, const Vector *u, float_t t)
{
	Vector w;
	
	bzero(&w, sizeof(w));
	w = vector_sub(u, v);
	w = vector_mul(&w, t);
	w = vector_add(v, &w);
	
	return w;
}

Vector
vector_slerp(const Vector *v, const Vector *u, float_t t)
{
	const double DOT_THRESHOLD = 0.9995;
	
	float_t dot;
	float_t theta;
	Vector v0;
	Vector u0;
	Vector w;
	
	v0 = *v;
	u0 = *u;
	dot = vector_dot(&v0, &u0);
	
    if (dot > DOT_THRESHOLD) {
		vector_lerp(&v0, &u0, t);
    }
	
	theta = t * acosf(dot);
	
	w = vector_mul(v, dot);
	w = vector_sub(u, &w);
	//w = vector_normalize(&w);
	
	v0 = vector_mul(&v0, cosf(theta));
	u0 = vector_mul(&u0, sinf(theta));
	w = vector_add(&v0, &u0);
	
	return w;
}
