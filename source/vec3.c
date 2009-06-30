/*-
 * License
 */

#include <strings.h>

#include "vec3.h"

vec3
vec3_add(const vec3 *v, const vec3 *u)
{
	vec3 w;
	w.x = v->x + u->x;
	w.y = v->y + u->y;
	w.z = v->z + u->z;
	return w;
}

vec3
vec3_cross(const vec3 *v, const vec3 *u)
{
	/*e1 e2 e3
	 x  y  z
	 x  y  z*/
	
	vec3 w;
	w.x = v->y * u->z - v->z * u->y;
	w.y = v->x * u->z - v->z * u->x;
	w.z = v->x * u->y - v->y * u->x;
	return w;
}

float_t
vec3_dot(const vec3 *v, const vec3 *u)
{
	return (v->x * u->x) + (v->y * u->y) + (v->z * u->z);
}

float_t
vec3_length(const vec3 *v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

vec3
vec3_lerp(const vec3 *v, const vec3 *u, float_t t)
{
	vec3 w;
	
	bzero(&w, sizeof(w));
	w = vec3_sub(u, v);
	w = vec3_mul(&w, t);
	w = vec3_add(v, &w);
	
	return w;
}

vec3
vec3_mul(const vec3 *v, float_t scale)
{
	vec3 w;
	w.x = scale * v->x;
	w.y = scale * v->y;
	w.z = scale * v->z;
	return w;
}

vec3
vec3_normalize(const vec3 *v)
{
	float_t len = vec3_length(v);
	vec3 u;
	
	u.x = v->x / len;
	u.y = v->y / len;
	u.z = v->z / len;
	
	return u;
}

/* TODO: Implement .z */
vec3
vec3_project(const vec3 *v, const vec3 *u)
{
	vec3 w;
	float_t tmp;

	tmp = ((v->x * u->x + v->y * u->y) / (pow(u->x,2) + pow(u->y,2)));
	w.x = tmp * u->x;
	w.y = tmp * u->y;

	return w;
}

/* FIXME: Broken, entities "disappears" */
vec3
vec3_slerp(const vec3 *v, const vec3 *u, float_t t)
{
	const double DOT_THRESHOLD = 0.9995;
	
	float_t dot;
	float_t theta;
	vec3	v0,
			u0,
			w;
	
	v0 = *v;
	u0 = *u;
	dot = vec3_dot(&v0, &u0);
	
    if (dot > DOT_THRESHOLD) {
		vec3_lerp(&v0, &u0, t);
    }
	
	theta = t * acosf(dot);
	
	w = vec3_mul(v, dot);
	w = vec3_sub(u, &w);
	//w = vector_normalize(&w);
	
	v0 = vec3_mul(&v0, cosf(theta));
	u0 = vec3_mul(&u0, sinf(theta));
	w = vec3_add(&v0, &u0);
	
	return w;
}

vec3
vec3_sub(const vec3 *v, const vec3 *u)
{
	vec3 w;
	
	w.x = v->x - u->x;
	w.y = v->y - u->y;
	w.z = v->z - u->z;
	return w;
}

