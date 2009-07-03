/*-
 * License
 */

#include <stdio.h>

#include "mat4.h"
#include "vec3.h"

static void
swap(float_t *x, float_t *y)
{
	float_t tmp;
	
	tmp = *x;
	*x = *y;
	*y = tmp;
}

void
mat4_copy(const mat4 *in, mat4 *m)
{
	
	m->m[0][0]=in->m[0][0];
	m->m[0][1]=in->m[0][1];
	m->m[0][2]=in->m[0][2];
	m->m[0][3]=in->m[0][3];
	m->m[1][0]=in->m[1][0];
	m->m[1][1]=in->m[1][1];
	m->m[1][2]=in->m[1][2];
	m->m[1][3]=in->m[1][3];
	m->m[2][0]=in->m[2][0];
	m->m[2][1]=in->m[2][1];
	m->m[2][2]=in->m[2][2];
	m->m[2][3]=in->m[2][3];
	m->m[3][0]=in->m[3][0];
	m->m[3][1]=in->m[3][1];
	m->m[3][2]=in->m[3][2];
	m->m[3][3]=in->m[3][3];
}

float_t
mat4_determinant(mat4 *m)
{
	float_t a00 = m->m[1][1] * m->m[2][2] * m->m[3][3] - m->m[1][1] * m->m[2][3] * m->m[3][2]
	- m->m[1][2] * m->m[2][1] * m->m[3][3] + m->m[1][2] * m->m[2][3] * m->m[3][1]
	+ m->m[1][3] * m->m[2][1] * m->m[3][2] - m->m[1][3] * m->m[2][2] * m->m[3][1];
	
	a00 = m->m[0][0] * a00;
	
	float_t a01 = m->m[1][0] * m->m[2][2] * m->m[3][3] - m->m[1][0] * m->m[2][3] * m->m[3][2]
	- m->m[1][2] * m->m[2][0] * m->m[3][3] + m->m[1][2] * m->m[2][3] * m->m[3][0]
	+ m->m[1][3] * m->m[2][0] * m->m[3][2] - m->m[1][3] * m->m[2][2] * m->m[3][0];
	
	a01 = m->m[0][1] * a01;
	
	float_t a02 = m->m[1][0] * m->m[2][1] * m->m[3][3] - m->m[1][0] * m->m[2][3] * m->m[3][1]
	- m->m[1][1] * m->m[2][0] * m->m[3][3] + m->m[1][1] * m->m[2][3] * m->m[3][0]
	+ m->m[1][3] * m->m[2][0] * m->m[3][1] - m->m[1][3] * m->m[2][1] * m->m[3][0];
	
	a02 = m->m[0][2] * a02;
	
	float_t a03 = m->m[1][0] * m->m[2][1] * m->m[3][2] - m->m[1][0] * m->m[2][2] * m->m[3][1]
	- m->m[1][1] * m->m[2][0] * m->m[3][2] + m->m[1][1] * m->m[2][2] * m->m[3][0]
	+ m->m[1][2] * m->m[2][0] * m->m[3][1] - m->m[1][2] * m->m[2][1] * m->m[3][0];
	
	a03 = m->m[0][3] * a03;
	
	return (a00 + a01 + a02 + a03);
}

void
mat4_identity(mat4 *m)
{	
	m->m[0][0]=m->m[1][1]=m->m[2][2]=m->m[3][3]=1.0f;
	m->m[0][1]=m->m[0][2]=m->m[0][3]=0.0f;
	m->m[1][0]=m->m[1][2]=m->m[1][3]=0.0f;
	m->m[2][0]=m->m[2][1]=m->m[2][3]=0.0f;
	m->m[3][0]=m->m[3][1]=m->m[3][2]=0.0f;
}

/* TODO: Implement */
void
mat4_inverse(const mat4 *m, mat4 *out)
{
}

void
mat4_lerp(const mat4 *m, const mat4 *in, mat4 *out, float t)
{
	
	out->m[0][0] = m->m[0][0] + ((m->m[0][0] - in->m[0][0]) * t);
	out->m[0][1] = m->m[0][1] + ((m->m[0][1] - in->m[0][1]) * t);
	out->m[0][2] = m->m[0][2] + ((m->m[0][2] - in->m[0][2]) * t);
	out->m[0][3] = m->m[0][3] + ((m->m[0][3] - in->m[0][3]) * t);
	out->m[1][0] = m->m[1][0] + ((m->m[1][0] - in->m[1][0]) * t);
	out->m[1][1] = m->m[1][1] + ((m->m[1][1] - in->m[1][1]) * t);
	out->m[1][2] = m->m[1][2] + ((m->m[1][2] - in->m[1][2]) * t);
	out->m[1][3] = m->m[1][3] + ((m->m[1][3] - in->m[1][3]) * t);
	out->m[2][0] = m->m[2][0] + ((m->m[2][0] - in->m[2][0]) * t);
	out->m[2][1] = m->m[2][1] + ((m->m[2][1] - in->m[2][1]) * t);
	out->m[2][2] = m->m[2][2] + ((m->m[2][2] - in->m[2][2]) * t);
	out->m[2][3] = m->m[2][3] + ((m->m[2][3] - in->m[2][3]) * t);
	out->m[3][0] = m->m[3][0] + ((m->m[3][0] - in->m[3][0]) * t);
	out->m[3][1] = m->m[3][1] + ((m->m[3][1] - in->m[3][1]) * t);
	out->m[3][2] = m->m[3][2] + ((m->m[3][2] - in->m[3][2]) * t);
	out->m[3][3] = m->m[3][3] + ((m->m[3][3] - in->m[3][3]) * t);
}

void
mat4_mul(const mat4 *m, const mat4 *in, mat4 *out)
{
	mat4 tmp;
	
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			tmp.m[i][j] = m->m[i][0]*in->m[0][j] + m->m[i][1]*in->m[1][j] +
			m->m[i][2]*in->m[2][j] + m->m[i][3]*in->m[3][j];
		}
	}
	
	mat4_copy(&tmp, out);
}

void
mat4_mul_vec3(const mat4 *m, const vec3 *v, float w, vec3 *out)
{
	vec3 tmp;

	tmp.x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z +
		m->m[3][0] * w;
	tmp.y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z +
		m->m[3][1] * w;
	tmp.z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z +
		m->m[3][2] * w;
	
	*out = tmp;
}

void
mat4_print(const mat4 *m)
{
	
	for(int i=0; i<16; i++)
	{
		if (!(i%4)) {
			printf("\n");
		}
		
		printf("%.2f, ", m->m[i/4][i%4]);
	}
	printf("\n");
}

void
mat4_reset(mat4 *m)
{
	m->m[0][0]=m->m[1][1]=m->m[2][2]=m->m[3][3]=0.0f;
	m->m[0][1]=m->m[0][2]=m->m[0][3]=0.0f;
	m->m[1][0]=m->m[1][2]=m->m[1][3]=0.0f;
	m->m[2][0]=m->m[2][1]=m->m[2][3]=0.0f;
	m->m[3][0]=m->m[3][1]=m->m[3][2]=0.0f;
}


void
mat4_rotate(mat4 *m, float angle, float x, float y, float z)
{
	mat4 tmp;
	
	mat4_rotation(&tmp, angle, x, y, z);
	mat4_mul(m, &tmp, &tmp);
	mat4_copy(&tmp, m);
}

void
mat4_rotation(mat4 *m, float angle, float x, float y, float z)
{
	const float DEG2RAD = M_PI / 180.0f;
	float fcos = cosf(angle * DEG2RAD);
	float fsin = sinf(angle * DEG2RAD);
	vec3 dir = {x, y, z};
	
	dir = vec3_normalize(&dir);
	m->m[0][0] = (dir.x * dir.x) * (1.0f - fcos) + fcos;
	m->m[0][1] = (dir.x * dir.y) * (1.0f - fcos ) - (dir.z * fsin);
	m->m[0][2] = (dir.x * dir.z) * (1.0f - fcos ) + (dir.y * fsin);
	
	m->m[1][0] = (dir.y * dir.x) * (1.0f - fcos ) + (dir.z * fsin);
	m->m[1][1] = (dir.y * dir.y) * (1.0f - fcos ) + fcos ;
	m->m[1][2] = (dir.y * dir.z) * (1.0f - fcos ) - (dir.x * fsin);
	
	m->m[2][0] = (dir.z * dir.x) * (1.0f - fcos ) - (dir.y * fsin);
	m->m[2][1] = (dir.z * dir.y) * (1.0f - fcos ) + (dir.x * fsin);
	m->m[2][2] = (dir.z * dir.z) * (1.0f - fcos ) + fcos;
	
	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0;
	m->m[3][3] = 1;
}

void
mat4_transpose(mat4 *m)
{
	swap(&m->m[0][1], &m->m[1][2]);
	swap(&m->m[0][2], &m->m[2][2]);
	swap(&m->m[0][3], &m->m[3][0]);
	swap(&m->m[1][2], &m->m[2][1]);
	swap(&m->m[1][3], &m->m[3][1]);
	swap(&m->m[2][3], &m->m[3][2]);
}
