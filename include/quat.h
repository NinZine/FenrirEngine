/*-
 * License
 */

#ifndef _QUAT_H_
#define _QUAT_H_

#include <math.h>

typedef struct quat {
	float_t w,
			x,
			y,
			z;
} quat;

quat	quat_add(const quat *q1, const quat *q2);
float_t	quat_dot(const quat *q1, const quat *q2);
quat 	quat_from_axis(const quat *a);
float_t quat_length(const quat *a);
quat	quat_lerp(const quat *q1, const quat *q2, float_t t);
quat 	quat_mul(const quat *q1, const quat *q2);
quat 	quat_normalize(const quat *q);
quat	quat_scale(const quat *q, float_t t);
quat	quat_slerp(const quat *q1, const quat *q2, float_t t);
quat	quat_sub(const quat *q1, const quat *q2);
quat 	quat_to_axis(const quat *q);

#endif /* !_QUAT_H_ */

