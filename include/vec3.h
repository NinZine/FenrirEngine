/*
 *  Math.h
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/13/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _VEC3_H_
#define _VEC3_H_

#include <math.h>

typedef struct vec3 {
	float_t x,
			y,
			z;
} vec3;


vec3 	vec3_add(const vec3 *v, const vec3 *u);
vec3 	vec3_cross(const vec3 *v, const vec3 *u);
float_t vec3_dot(const vec3 *v, const vec3 *u);
float_t vec3_length(const vec3 *v);
vec3	vec3_lerp(const vec3 *v, const vec3 *u, float_t t);
vec3	vec3_mul(const vec3 *v, float_t scale);
vec3	vec3_normalize(const vec3 *v);
vec3	vec3_project(const vec3 *v, const vec3 *u);
vec3	vec3_slerp(const vec3 *v, const vec3 *u, float_t t);
vec3	vec3_sub(const vec3 *v, const vec3 *u);

#endif /* !_VEC3_H_ */

