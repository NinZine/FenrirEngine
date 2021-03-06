/*-
 * License
 */

#include <stdio.h>
#include <string.h>

#include "mat4.h"
#include "vec3.h"

static void
swap(float *x, float *y)
{
	float tmp;
	
	tmp = *x;
	*x = *y;
	*y = tmp;
}

mat4
mat4_copy(const mat4 *in)
{
	mat4 m;

	memcpy(m.m, in->m, sizeof(float)*16);

    return m;
}

float
mat4_determinant(mat4 *m)
{
	float a00, a01, a02, a03;

	a00 = m->m[1][1] * m->m[2][2] * m->m[3][3] - m->m[1][1] * m->m[2][3] * m->m[3][2]
	- m->m[1][2] * m->m[2][1] * m->m[3][3] + m->m[1][2] * m->m[2][3] * m->m[3][1]
	+ m->m[1][3] * m->m[2][1] * m->m[3][2] - m->m[1][3] * m->m[2][2] * m->m[3][1];
	
	a00 = m->m[0][0] * a00;
	
	a01 = m->m[1][0] * m->m[2][2] * m->m[3][3] - m->m[1][0] * m->m[2][3] * m->m[3][2]
	- m->m[1][2] * m->m[2][0] * m->m[3][3] + m->m[1][2] * m->m[2][3] * m->m[3][0]
	+ m->m[1][3] * m->m[2][0] * m->m[3][2] - m->m[1][3] * m->m[2][2] * m->m[3][0];
	
	a01 = m->m[0][1] * a01;
	
	a02 = m->m[1][0] * m->m[2][1] * m->m[3][3] - m->m[1][0] * m->m[2][3] * m->m[3][1]
	- m->m[1][1] * m->m[2][0] * m->m[3][3] + m->m[1][1] * m->m[2][3] * m->m[3][0]
	+ m->m[1][3] * m->m[2][0] * m->m[3][1] - m->m[1][3] * m->m[2][1] * m->m[3][0];
	
	a02 = m->m[0][2] * a02;
	
	a03 = m->m[1][0] * m->m[2][1] * m->m[3][2] - m->m[1][0] * m->m[2][2] * m->m[3][1]
	- m->m[1][1] * m->m[2][0] * m->m[3][2] + m->m[1][1] * m->m[2][2] * m->m[3][0]
	+ m->m[1][2] * m->m[2][0] * m->m[3][1] - m->m[1][2] * m->m[2][1] * m->m[3][0];
	
	a03 = m->m[0][3] * a03;
	
	return (a00 + a01 + a02 + a03);
}

mat4
mat4_identity()
{
    mat4 m;

	memset(m.m, 0, sizeof(float)*16);
	m.m[0][0]=m.m[1][1]=m.m[2][2]=m.m[3][3]=1.0f;
    
    return m;
}

/* TODO: Implement */
mat4
mat4_inverse(const mat4 *m)
{
    mat4 out;

    return out;
}

mat4
mat4_lerp(const mat4 *m, const mat4 *in, float t)
{
	mat4 out;

	/*
	out.m[0][0] = m->m[0][0] * t + (in->m[0][0] * (1.0f - t));
	out.m[0][1] = m->m[0][1] * t + (in->m[0][1] * (1.0f - t));
	out.m[0][2] = m->m[0][2] * t + (in->m[0][2] * (1.0f - t));
	out.m[0][3] = m->m[0][3] * t + (in->m[0][3] * (1.0f - t));
	out.m[1][0] = m->m[1][0] * t + (in->m[1][0] * (1.0f - t));
	out.m[1][1] = m->m[1][1] * t + (in->m[1][1] * (1.0f - t));
	out.m[1][2] = m->m[1][2] * t + (in->m[1][2] * (1.0f - t));
	out.m[1][3] = m->m[1][3] * t + (in->m[1][3] * (1.0f - t));
	out.m[2][0] = m->m[2][0] * t + (in->m[2][0] * (1.0f - t));
	out.m[2][1] = m->m[2][1] * t + (in->m[2][1] * (1.0f - t));
	out.m[2][2] = m->m[2][2] * t + (in->m[2][2] * (1.0f - t));
	out.m[2][3] = m->m[2][3] * t + (in->m[2][3] * (1.0f - t));
	out.m[3][0] = m->m[3][0] * t + (in->m[3][0] * (1.0f - t));
	out.m[3][1] = m->m[3][1] * t + (in->m[3][1] * (1.0f - t));
	out.m[3][2] = m->m[3][2] * t + (in->m[3][2] * (1.0f - t));
	out.m[3][3] = m->m[3][3] * t + (in->m[3][3] * (1.0f - t));
	*/

	out.m[0][0] = m->m[0][0] + ((in->m[0][0] - m->m[0][0]) * t);
	out.m[0][1] = m->m[0][1] + ((in->m[0][1] - m->m[0][1]) * t);
	out.m[0][2] = m->m[0][2] + ((in->m[0][2] - m->m[0][2]) * t);
	out.m[0][3] = m->m[0][3] + ((in->m[0][3] - m->m[0][3]) * t);
	out.m[1][0] = m->m[1][0] + ((in->m[1][0] - m->m[1][0]) * t);
	out.m[1][1] = m->m[1][1] + ((in->m[1][1] - m->m[1][1]) * t);
	out.m[1][2] = m->m[1][2] + ((in->m[1][2] - m->m[1][2]) * t);
	out.m[1][3] = m->m[1][3] + ((in->m[1][3] - m->m[1][3]) * t);
	out.m[2][0] = m->m[2][0] + ((in->m[2][0] - m->m[2][0]) * t);
	out.m[2][1] = m->m[2][1] + ((in->m[2][1] - m->m[2][1]) * t);
	out.m[2][2] = m->m[2][2] + ((in->m[2][2] - m->m[2][2]) * t);
	out.m[2][3] = m->m[2][3] + ((in->m[2][3] - m->m[2][3]) * t);
	out.m[3][0] = m->m[3][0] + ((in->m[3][0] - m->m[3][0]) * t);
	out.m[3][1] = m->m[3][1] + ((in->m[3][1] - m->m[3][1]) * t);
	out.m[3][2] = m->m[3][2] + ((in->m[3][2] - m->m[3][2]) * t);
	out.m[3][3] = m->m[3][3] + ((in->m[3][3] - m->m[3][3]) * t);

    return out;
}

mat4
mat4_mul(const mat4 *m, const mat4 *in)
{
	mat4 tmp;
	
	/*
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			tmp.m[i][j] = m->m[i][0]*in->m[0][j] + m->m[i][1]*in->m[1][j] +
			m->m[i][2]*in->m[2][j] + m->m[i][3]*in->m[3][j];
		}
	}*/
	
	/* Optimized */
	tmp.m[0][0] = m->m[0][0]*in->m[0][0]+m->m[0][1]*in->m[1][0]
		+m->m[0][2]*in->m[2][0]+m->m[0][3]*in->m[3][0];
	tmp.m[0][1] = m->m[0][0]*in->m[0][1]+m->m[0][1]*in->m[1][1]
		+m->m[0][2]*in->m[2][1]+m->m[0][3]*in->m[3][1];
	tmp.m[0][2] = m->m[0][0]*in->m[0][2]+m->m[0][1]*in->m[1][2]
		+m->m[0][2]*in->m[2][2]+m->m[0][3]*in->m[3][2];
	tmp.m[0][3] = m->m[0][0]*in->m[0][3]+m->m[0][1]*in->m[1][3]
		+m->m[0][2]*in->m[2][3]+m->m[0][3]*in->m[3][3];
	
	tmp.m[1][0] = m->m[1][0]*in->m[0][0]+m->m[1][1]*in->m[1][0]
		+m->m[1][2]*in->m[2][0]+m->m[1][3]*in->m[3][0];
	tmp.m[1][1] = m->m[1][0]*in->m[0][1]+m->m[1][1]*in->m[1][1]
		+m->m[1][2]*in->m[2][1]+m->m[1][3]*in->m[3][1];
	tmp.m[1][2] = m->m[1][0]*in->m[0][2]+m->m[1][1]*in->m[1][2]
		+m->m[1][2]*in->m[2][2]+m->m[1][3]*in->m[3][2];
	tmp.m[1][3] = m->m[1][0]*in->m[0][3]+m->m[1][1]*in->m[1][3]
		+m->m[1][2]*in->m[2][3]+m->m[1][3]*in->m[3][3];

	tmp.m[2][0] = m->m[2][0]*in->m[0][0]+m->m[2][1]*in->m[1][0]
		+m->m[2][2]*in->m[2][0]+m->m[2][3]*in->m[3][0];
	tmp.m[2][1] = m->m[2][0]*in->m[0][1]+m->m[2][1]*in->m[1][1]
		+m->m[2][2]*in->m[2][1]+m->m[2][3]*in->m[3][1];
	tmp.m[2][2] = m->m[2][0]*in->m[0][2]+m->m[2][1]*in->m[1][2]
		+m->m[2][2]*in->m[2][2]+m->m[2][3]*in->m[3][2];
	tmp.m[2][3] = m->m[2][0]*in->m[0][3]+m->m[2][1]*in->m[1][3]
		+m->m[2][2]*in->m[2][3]+m->m[2][3]*in->m[3][3];

	tmp.m[3][0] = m->m[3][0]*in->m[0][0]+m->m[3][1]*in->m[1][0]
		+m->m[3][2]*in->m[2][0]+m->m[3][3]*in->m[3][0];
	tmp.m[3][1] = m->m[3][0]*in->m[0][1]+m->m[3][1]*in->m[1][1]
		+m->m[3][2]*in->m[2][1]+m->m[3][3]*in->m[3][1];
	tmp.m[3][2] = m->m[3][0]*in->m[0][2]+m->m[3][1]*in->m[1][2]
		+m->m[3][2]*in->m[2][2]+m->m[3][3]*in->m[3][2];
	tmp.m[3][3] = m->m[3][0]*in->m[0][3]+m->m[3][1]*in->m[1][3]
		+m->m[3][2]*in->m[2][3]+m->m[3][3]*in->m[3][3];

    return tmp;
}

vec3
mat4_mul_vec3(const mat4 *m, const vec3 *v, float w)
{
	vec3 tmp;

	tmp.x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z +
		m->m[3][0] * w;
	tmp.y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z +
		m->m[3][1] * w;
	tmp.z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z +
		m->m[3][2] * w;
	
    return tmp;
}

void
mat4_print(const mat4 *m)
{
	int i;
	for(i=0; i<16; i++)
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
	
	memset(m->m, 0, sizeof(float)*16);
}


mat4
mat4_rotate(const mat4 *m, float angle, float x, float y, float z)
{
	mat4 tmp;
	
    tmp = mat4_identity();
	tmp = mat4_rotation(angle, x, y, z);
	tmp = mat4_mul(&tmp, m);
    
    return tmp;
}

mat4
mat4_rotation(float angle, float x, float y, float z)
{
    mat4 m;
	const float DEG2RAD = M_PI / 180.0f;
	float fcos = cosf(angle * DEG2RAD);
	float fsin = sinf(angle * DEG2RAD);
	vec3 dir = {x, y, z};
	
	dir = vec3_normalize(&dir);
	m.m[0][0] = (dir.x * dir.x) * (1.0f - fcos) + fcos;
	m.m[0][1] = (dir.x * dir.y) * (1.0f - fcos ) - (dir.z * fsin);
	m.m[0][2] = (dir.x * dir.z) * (1.0f - fcos ) + (dir.y * fsin);
	
	m.m[1][0] = (dir.y * dir.x) * (1.0f - fcos ) + (dir.z * fsin);
	m.m[1][1] = (dir.y * dir.y) * (1.0f - fcos ) + fcos ;
	m.m[1][2] = (dir.y * dir.z) * (1.0f - fcos ) - (dir.x * fsin);
	
	m.m[2][0] = (dir.z * dir.x) * (1.0f - fcos ) - (dir.y * fsin);
	m.m[2][1] = (dir.z * dir.y) * (1.0f - fcos ) + (dir.x * fsin);
	m.m[2][2] = (dir.z * dir.z) * (1.0f - fcos ) + fcos;
	
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0;
	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0;
	m.m[3][3] = 1;

    return m;
}

mat4
mat4_scale(const mat4 *m, float x, float y, float z)
{
    mat4 n;

    n = mat4_identity();
    n.m[0][0] = m->m[0][0] * x;
    n.m[1][1] = m->m[1][1] * y;
    n.m[2][2] = m->m[2][2] * z;

    return n;
}

mat4
mat4_translate(const mat4 *m, float x, float y, float z)
{
    mat4 n;

    n = mat4_copy(m);
    
    n.m[3][0] = m->m[3][0] + x;
    n.m[3][1] = m->m[3][1] + y;
    n.m[3][2] = m->m[3][2] + z;

    return n;
}

mat4
mat4_transpose()
{
	mat4 m;
	
	swap(&m.m[0][1], &m.m[1][2]);
	swap(&m.m[0][2], &m.m[2][2]);
	swap(&m.m[0][3], &m.m[3][0]);
	swap(&m.m[1][2], &m.m[2][1]);
	swap(&m.m[1][3], &m.m[3][1]);
	swap(&m.m[2][3], &m.m[3][2]);
	
	return m;
}

