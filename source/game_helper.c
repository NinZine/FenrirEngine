/*-
 * License
 */

#include <sys/time.h>

#if __NDS__ == 1
# include <nds.h>
# include <nds/timers.h>
#endif /* !__NDS__ */
#include <err.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "game_helper.h"

static gh_button button[10];
static unsigned int buttons = 10;

void
gh_build_mat4(struct gh_rigid_body *obj, mat4 *out)
{
	mat4 pos, rot, sca;
	
	quat_to_mat4(&obj->rotation, &rot);
	mat4_identity(&pos);
	pos.m[3][0] = obj->position.x;
	pos.m[3][1] = obj->position.y;
	pos.m[3][2] = obj->position.z;
	mat4_identity(&sca);
	sca.m[0][0] = obj->scale.x;
	sca.m[1][1] = obj->scale.y;
	sca.m[2][2] = obj->scale.z;
	mat4_mul(&sca, &rot, &rot);
	mat4_mul(&rot, &pos, &pos); /* Translation matrix in pos */
	
	*out = pos;
}

bool
gh_collides(const vec3 *edge, const int num_edges, const vec3 *poly1,
	uint8_t n1, const vec3 *poly2, uint8_t n2, float_t *min_dist, int *axis)
{
	bool first = true;
	bool collide = true;
	int k;

	*min_dist = 0.f;
	*axis = -1;
	
	for (k = 0; k < num_edges; ++k) {
		float_t min1, max1, min2, max2, dist;
		
		/* Project both polygons on current edge */
		gh_project_vec3(&edge[k], poly1, n1, &min1, &max1);
		gh_project_vec3(&edge[k], poly2, n2, &min2, &max2);
		
		if (min1 < min2) {
			dist = min2 - max1;
		} else {
			dist = min1 - max2;
		}
		
		/* No collision on this axis, so no collision at all */
		if (dist > 0) {
			collide = false;
			break;
		}
		
		/* Collision, get minimum distance/axis */
		dist = fabs(dist);
		if (first || dist < *min_dist) {
			first = false;
			*min_dist = dist;
			*axis = k;
		}
	}
	
	return collide;
}

void
gh_copy_state(struct gh_state *dest, struct gh_state *src, bool use_malloc)
{
	
	if (0 != (src->count - dest->count) ){
		gh_array_resize((void**)&dest->object, dest->count,
			sizeof(struct gh_rigid_body), src->count - dest->count);
	}
	dest->count = src->count;
	memcpy(dest->object, src->object, sizeof(gh_rigid_body) * dest->count);
}

void
gh_create_model(gh_model *m, enum gh_shape shape, ...)
{
	static vec3 circle[63] = {
		{1.00f, -0.00f, 0.f},
		{0.99f, -0.10f, 0.f},
		{0.98f, -0.20f, 0.f},
		{0.95f, -0.30f, 0.f},
		{0.92f, -0.39f, 0.f},
		{0.88f, -0.48f, 0.f},
		{0.82f, -0.57f, 0.f},
		{0.76f, -0.65f, 0.f},
		{0.69f, -0.72f, 0.f},
		{0.62f, -0.79f, 0.f},
		{0.54f, -0.84f, 0.f},
		{0.45f, -0.89f, 0.f},
		{0.36f, -0.93f, 0.f},
		{0.26f, -0.96f, 0.f},
		{0.17f, -0.99f, 0.f},
		{0.07f, -1.00f, 0.f},
		{-0.03f, -1.00f, 0.f},
		{-0.13f, -0.99f, 0.f},
		{-0.23f, -0.97f, 0.f},
		{-0.33f, -0.95f, 0.f},
		{-0.42f, -0.91f, 0.f},
		{-0.51f, -0.86f, 0.f},
		{-0.59f, -0.81f, 0.f},
		{-0.67f, -0.74f, 0.f},
		{-0.74f, -0.67f, 0.f},
		{-0.80f, -0.60f, 0.f},
		{-0.86f, -0.51f, 0.f},
		{-0.91f, -0.42f, 0.f},
		{-0.94f, -0.33f, 0.f},
		{-0.97f, -0.24f, 0.f},
		{-0.99f, -0.14f, 0.f},
		{-1.00f, -0.04f, 0.f},
		{-1.00f, 0.06f, 0.f},
		{-0.99f, 0.16f, 0.f},
		{-0.97f, 0.26f, 0.f},
		{-0.94f, 0.35f, 0.f},
		{-0.90f, 0.45f, 0.f},
		{-0.85f, 0.53f, 0.f},
		{-0.79f, 0.61f, 0.f},
		{-0.72f, 0.69f, 0.f},
		{-0.65f, 0.76f, 0.f},
		{-0.57f, 0.82f, 0.f},
		{-0.49f, 0.87f, 0.f},
		{-0.40f, 0.92f, 0.f},
		{-0.30f, 0.95f, 0.f},
		{-0.21f, 0.98f, 0.f},
		{-0.11f, 0.99f, 0.f},
		{-0.01f, 1.00f, 0.f},
		{0.09f, 1.00f, 0.f},
		{0.19f, 0.98f, 0.f},
		{0.29f, 0.96f, 0.f},
		{0.38f, 0.92f, 0.f},
		{0.47f, 0.88f, 0.f},
		{0.56f, 0.83f, 0.f},
		{0.64f, 0.77f, 0.f},
		{0.71f, 0.70f, 0.f},
		{0.78f, 0.63f, 0.f},
		{0.84f, 0.55f, 0.f},
		{0.89f, 0.46f, 0.f},
		{0.93f, 0.37f, 0.f},
		{0.96f, 0.28f, 0.f},
		{0.98f, 0.18f, 0.f},
		{1.00f, 0.08f, 0.f},
	};
	static vec3 quad_edge[2] = {
		{1.f, 0.f, 0.f},
		{0.f, 1.f, 0.f},
	};
	static vec3 quad[4] = {
		{-0.5f, -0.5f, 0.f},
		{ 0.5f, -0.5f, 0.f},
		{-0.5f,  0.5f, 0.f},
		{ 0.5f,  0.5f, 0.f},
	};
	va_list ap;
	
	m->shape = shape;
	va_start(ap, shape);
	if (S_CIRCLE == shape) {
		m->vertex = circle;
		m->vertices = 63;
	} else if (S_POLYGON == shape) {
		vec3 *point;
		vec3 *edge;
		
		point = va_arg(ap, vec3*);
		m->vertices = va_arg(ap, int);
		edge = va_arg(ap, vec3*);
		m->edges = va_arg(ap, int);
		gh_array_resize((void**)&m->vertex, 0, sizeof(vec3), m->vertices);
		gh_array_resize((void**)&m->edge, 0, sizeof(vec3), m->edges);
		memcpy(m->vertex, point, m->vertices * sizeof(vec3));
		memcpy(m->edge, edge, m->edges * sizeof(vec3));
	} else if (S_QUAD == shape) {
		m->vertex = quad;
		m->vertices = 4;
		m->edge = quad_edge;
		m->edges = 2;
	} else if (S_RAY == shape) {
	} else {
		err(0, "Failed to create shape!");
	}
	
	va_end(ap);
}

gh_button*
gh_get_input(const unsigned int n)
{
	
	if (n > buttons)
		return 0;
	
	return &button[n];
}

void
gh_input(gh_button *b, const unsigned int n)
{
	if (n >= buttons) {
		return;
	}
	
	button[n] = *b;
}

void
gh_project_vec3(const vec3 *axis, const vec3 *point, const int sz_points,
	float_t *min, float_t *max)
{
	float_t dot;
	int i = 0;
	
	dot = vec3_dot(axis, &point[i]);
	*min = dot;
	*max = dot;
	
	for (i = 1; i < sz_points; ++i) {
		dot = vec3_dot(axis, &point[i]);
		
		if (dot < *min) {
			*min = dot;
		} else if (dot > *max) {
			*max = dot;
		}
	}
}
float_t
gh_rad2deg(const float_t rad)
{
	static const float PI = 3.14159265359;
	return rad * (180.f / PI);
}

#if __NDS__ == 0
float_t
gh_time_elapsed()
{
	static struct timeval start = {0,0};
	struct timeval counter;
	
	if (0 == start.tv_sec && 0 == start.tv_usec) {
		gettimeofday(&start, 0);
		return 0.f;
	}
	
	gettimeofday(&counter, 0);
	counter.tv_sec = counter.tv_sec - start.tv_sec;
	counter.tv_usec = counter.tv_usec - start.tv_usec;
	
	float_t tmp = ((counter.tv_sec * 1000000) + counter.tv_usec) / 1000000.f;
    return tmp;
}
#else /* !__IPHONE__ */
float_t
gh_time_elapsed()
{
	static struct timeval counter = {0,0};
	static bool started = false;
	uint16_t tmp = 0;

	if (false == started) {
		timerStart(0, ClockDivider_1, 0, 0);
		
		//TIMER0_DATA = 0; //0xFFFF - 32768; 
		//TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
		started = true;
	}


	tmp = timerElapsed(0) / 33;
	counter.tv_usec += (tmp >= 993) ? tmp / 2 : tmp; //TIMER0_DATA / 33;
	while (counter.tv_usec > 100000) {
		counter.tv_sec += 1;
		counter.tv_usec -= 100000;
	}

	//printf("sec: %d ", counter.tv_sec);
	//printf("usec: %d\n", counter.tv_usec);
	return ((counter.tv_sec * 1000000) + counter.tv_usec) / 1000000.f;
}
#endif /* !__NDS__ */

void
gh_transform_edges(const mat4 *tf, vec3 *edge, const int num_edges)
{
	int i;
	
	for (i = 0; i < num_edges; ++i) {
		mat4_mul_vec3(tf, &edge[i], 0.f, &edge[i]);
		edge[i] = vec3_normalize(&edge[i]);
	}
}

void
gh_transform_vec3(const mat4 *tf, vec3 *v, const int num_points)
{
	int p;
	
	for (p = 0; p < num_points; ++p) {
		mat4_mul_vec3(tf, &v[p], 1.f, &v[p]);
	}
}

