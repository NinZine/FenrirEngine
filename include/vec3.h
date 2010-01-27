/*-
 * License
 */

#ifndef _VEC3_H_
#define _VEC3_H_

#include <math.h>

typedef struct vec3 {
	float x,
			y,
			z;
} vec3;


vec3 	vec3_add(const vec3 *v, const vec3 *u);
vec3 	vec3_cross(const vec3 *v, const vec3 *u);
float   vec3_dot(const vec3 *v, const vec3 *u);
float   vec3_length(const vec3 *v);
vec3	vec3_lerp(const vec3 *v, const vec3 *u, float t);
vec3	vec3_mul(const vec3 *v, float scale);
vec3	vec3_normalize(const vec3 *v);
vec3	vec3_project(const vec3 *v, const vec3 *u);
vec3	vec3_slerp(const vec3 *v, const vec3 *u, float t);
vec3	vec3_sub(const vec3 *v, const vec3 *u);

#endif /* !_VEC3_H_ */

