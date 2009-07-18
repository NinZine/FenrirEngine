/*-
 * License
 */

#include "quat.h"
#include "vec3.h"

static const float_t PI = 3.14159265359;

quat
quat_add(const quat *q1, const quat *q2)
{
	quat q;
	
	q.w = q1->w + q2->w;
	q.x = q1->x + q2->x;
	q.y = q1->y + q2->y;
	q.z = q1->z + q2->z;

	return q;
}

float_t
quat_dot(const quat *q1, const quat *q2)
{
	return q1->w*q2->w + q1->x*q2->x + q1->y*q2->y + q1->z*q2->z;
}

quat
quat_from_axis(const quat *a)
{
	float_t theta = (a->w * 0.5f) * (PI/180.f); /* To radians */
	float_t sin_theta = sin(theta);
	vec3 tmp = {a->x, a->y, a->z};
	quat q;
	
	//tmp = vec3_normalize(&tmp);
	q.w = cos(theta);
	q.x = tmp.x * sin_theta;
	q.y = tmp.y * sin_theta;
	q.z = tmp.z * sin_theta;
	q = quat_normalize(&q);
	
	return q;
}

float_t
quat_length(const quat *q)
{
	return sqrt(pow(q->w,2) + pow(q->x,2) + pow(q->y,2) + pow(q->z,2));
}

quat
quat_lerp(const quat *q1, const quat *q2, float_t t)
{
	quat q3, q4;

	q3 = quat_scale(q1, (1-t));
	q4 = quat_scale(q2, t);
	q3 = quat_add(&q3, &q4);

	return quat_normalize(&q3);
}

quat
quat_mul(const quat *q1, const quat *q2)
{
	quat tmp;

	tmp.w = q1->w*q2->w - q1->x*q2->x - q1->y*q2->y - q1->z*q2->z;
	tmp.x = q1->w*q2->x + q1->x*q2->w + q1->y*q2->z - q1->z*q2->y;
	tmp.y = q1->w*q2->y + q1->y*q2->w + q1->z*q2->x - q1->x*q2->z;
	tmp.z = q1->w*q2->z + q1->z*q2->w + q1->x*q2->y - q1->y*q2->x;

	return tmp;
}

quat
quat_normalize(const quat *q)
{
	float_t length;
	quat tmp = *q;

	length = quat_length(q);
	tmp.w /= length;
	tmp.x /= length;
	tmp.y /= length;
	tmp.z /= length;

	return tmp;
}

quat
quat_scale(const quat *q1, float_t t)
{
	quat q2;

	q2.w = q1->w * t;
	q2.x = q1->x * t;
	q2.y = q1->y * t;
	q2.z = q1->z * t;

	return q2;
}

quat
quat_slerp(const quat *q1, const quat *q2, float_t t)
{
	float_t cos_theta = quat_dot(q1, q2);
	float_t theta = acos(cos_theta);
	float_t sin_theta = sin(theta);

	if (sin_theta > 0.001f) {
		float_t w1 = sin( (1.f-t) * theta) / sin_theta;
		float_t w2 = sin(t * theta) / sin_theta;
		quat q3, q4;

		q3 = quat_scale(q1, w1);
		q4 = quat_scale(q2, w2);
		return quat_add(&q3, &q4);
	} else {
		return quat_lerp(q1, q2, t);
	}
}

quat
quat_sub(const quat *q1, const quat *q2)
{
	quat q;
	
	q.w = q1->w - q2->w;
	q.x = q1->x - q2->x;
	q.y = q1->y - q2->y;
	q.z = q1->z - q2->z;

	return q;
}

quat
quat_to_axis(const quat *q)
{
	vec3 axis = {q->x, q->y, q->z};
	quat tmp = {0.f, 1.f, 0.f, 0.f};
	float_t length;

	tmp.w = 2 * acos(q->w);
	//if ((int)(tmp.w) > 0 || (int)(tmp.w) < 0) {
		length = vec3_length(&axis);
		tmp.x = q->x / length;
		tmp.y = q->y / length;
		tmp.z = q->z / length;
		tmp.w = tmp.w * (180.f/PI); /* To degrees */
	//}

	return tmp;
}

void
quat_to_mat4(const quat *q, mat4 *m)
{
	float x2 = 2.0f * q->x,  y2 = 2.0f * q->y,  z2 = 2.0f * q->z;
	float xx = x2 * q->x,  xw = x2 * q->w,  yz = y2 * q->z;
	float xy = x2 * q->y,  xz = x2 * q->z;
	float yy = y2 * q->y,  yw = y2 * q->w;
	float zw = z2 * q->w,  zz = z2 * q->z;
	
	m->m[0][0] = 1.0f - (yy + zz);
	m->m[0][1] = (xy - zw);
	m->m[0][2] = (xz + yw);
	m->m[0][3] = 0.0f;
	
	m->m[1][0] = (xy +  zw);
	m->m[1][1] = 1.0f - (xx + zz);
	m->m[1][2] = (yz - xw);
	m->m[1][3] = 0.0f;
	
	m->m[2][0] = (xz - yw);
	m->m[2][1] = (yz + xw);
	m->m[2][2] = 1.0f - (xx + yy);  
	m->m[2][3] = 0.0f;  
	
	m->m[3][0] = 0.0f;
	m->m[3][1] = 0.0f; 
	m->m[3][2] = 0.0f; 
	m->m[3][3] = 1.0f;
}
