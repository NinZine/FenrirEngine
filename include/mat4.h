/*-
 * License
 */

#ifndef _MAT4_H_
#define _MAT4_H_

#include <math.h>
#include <stdbool.h>

#include "vec3.h"

typedef struct mat4 {
	float_t m[4][4];
} mat4;

void mat4_copy(const mat4 *in, mat4 *m);
float_t mat4_determinant(mat4 *m);
void mat4_identity(mat4 *m);
void mat4_lerp(const mat4 *m, const mat4 *in, mat4 *out, float t);
void mat4_mul(const mat4 *left, const mat4 *right, mat4 *out);
void mat4_mul_vec3(const mat4 *m, const vec3 *v, bool is_vector, vec3 *out);
void mat4_print(const mat4 *m);
void mat4_reset(mat4 *m);
void mat4_rotate(mat4 *m, float angle, float x, float y, float z);
void mat4_rotation(mat4 *m, float angle, float x, float y, float z);
void mat4_transpose(mat4 *m);

#endif /* !_MAT4_H_ */
