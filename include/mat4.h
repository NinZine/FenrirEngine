/*-
 * License
 */

#ifndef _MAT4_H_
#define _MAT4_H_

#include "vec3.h"

typedef struct mat4 {
	float m[4][4];
} mat4;

mat4 mat4_copy(const mat4 *in);
float mat4_determinant(mat4 *m);
mat4 mat4_identity();
mat4 mat4_inverse(const mat4 *m);
mat4 mat4_lerp(const mat4 *m, const mat4 *in, float t);
mat4 mat4_mul(const mat4 *left, const mat4 *right);
vec3 mat4_mul_vec3(const mat4 *m, const vec3 *v, float w);
void mat4_print(const mat4 *m);
void mat4_reset(mat4 *m);
mat4 mat4_rotate(const mat4 *m, float angle, float x, float y, float z);
mat4 mat4_rotation(float angle, float x, float y, float z);
mat4 mat4_scale(const mat4 *m, float x, float y, float z);
mat4 mat4_translate(const mat4 *m, float x, float y, float z);
mat4 mat4_transpose();

#endif /* !_MAT4_H_ */

