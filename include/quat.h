/*-
 * License
 */

#ifndef _QUAT_H_
#define _QUAT_H_

#include "mat4.h"

typedef struct quat {
	float w,
			x,
			y,
			z;
} quat;

quat	quat_add(const quat *q1, const quat *q2);
float	quat_dot(const quat *q1, const quat *q2);
quat 	quat_from_axis(const quat *a);
float   quat_length(const quat *a);
quat	quat_lerp(const quat *q1, const quat *q2, float t);
quat 	quat_mul(const quat *q1, const quat *q2);
quat 	quat_normalize(const quat *q);
quat	quat_scale(const quat *q, float t);
quat	quat_slerp(const quat *q1, const quat *q2, float t);
quat	quat_sub(const quat *q1, const quat *q2);
quat 	quat_to_axis(const quat *q);
mat4    quat_to_mat4(const quat *q);

#endif /* !_QUAT_H_ */

