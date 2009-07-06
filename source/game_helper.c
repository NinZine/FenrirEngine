/*-
 * License
 */

#include <sys/time.h>

#if __NDS__ == 1
# include <nds.h>
# include <nds/timers.h>
#endif /* !__NDS__ */
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "game_helper.h"

static gh_button *button;
static unsigned int buttons = 0;

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
	sca.m[0][0] = 20.f;
	sca.m[1][1] = 20.f;
	sca.m[2][2] = 20.f;
	mat4_mul(&sca, &rot, &rot);
	mat4_mul(&rot, &pos, &pos); /* Translation matrix in pos */
	
	*out = pos;
}

bool
gh_collides(const vec3 *edge, const int num_edges, const vec3 *poly1,
	const vec3 *poly2, float_t *min_dist, int *axis)
{
	bool first = true;
	bool collide = true;
	int k;

	*min_dist = 0.f;
	*axis = -1;
	
	for (k = 0; k < num_edges; ++k) {
		float_t min1, max1, min2, max2, dist;
		
		/* Project both polygons on current edge */
		gh_project_vec3(&edge[k], poly1, 4, &min1, &max1);
		gh_project_vec3(&edge[k], poly2, 4, &min2, &max2);
		
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
	int i;

	dest->count = src->count;

	if (true == use_malloc) {
		dest->object = malloc(sizeof(struct gh_rigid_body) * src->count);
	}

	for (i = 0; i < src->count; ++i) {

		dest->object[i].id = src->object[i].id;
		dest->object[i].position = src->object[i].position;
		dest->object[i].rotation = src->object[i].rotation;
		dest->object[i].linear_velocity = src->object[i].linear_velocity;
		dest->object[i].angular_velocity = src->object[i].angular_velocity;
	}
}

gh_button*
gh_get_input(const unsigned int n)
{
	
	if (n > buttons)
		return 0;
	
	return &button[n];
}

void
gh_input(struct gh_input *gi)
{
	if (gi->button >= buttons) {
		gh_array_resize((void**)&button, buttons,
						sizeof(struct gh_button), 1);
		buttons += 1;
	}
	
	button[gi->button] = *gi->data;
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

