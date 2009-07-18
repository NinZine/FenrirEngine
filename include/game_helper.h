/*-
 * License
 */

#ifndef _GAME_HELPER_H_
#define _GAME_HELPER_H_

#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "behavior.h"
#include "game_entity.h"
#include "mat4.h"
#include "quat.h"
#include "vec3.h"

typedef struct gh_button {
	vec3	position,
			rotation;
	float	size;
	int		held;
} gh_button;

struct gh_state
{
	int32_t count;
	struct gh_rigid_body *object;
};

typedef struct gh_time {
	float_t absolute,
			accumulator,
			delta,
			frame,
			now,
			timestep;
} gh_time;

void	gh_build_mat4(struct gh_rigid_body *obj, mat4 *out);
void	gh_copy_state(struct gh_state *dest, struct gh_state *src,
			bool use_malloc);
bool	gh_collides(const vec3 *edge, const int num_edges, const vec3 *poly1,
			uint8_t n1, const vec3 *poly2, uint8_t n2,
			float_t *min_dist, int *axis);
uint32_t gh_create_rigidbody(struct gh_state *state,
			vec3 *position, quat *rotation, vec3 *scale, vec3 *vel,
			vec3 *ang_vel);
gh_button*	gh_get_input(const unsigned int n);
void	gh_input(gh_button *button, const unsigned int n);
void	gh_project_vec3(const vec3 *axis, const vec3 *points,
			const int sz_points, float_t *min, float_t *max);
float_t gh_rad2deg(const float_t rad);
float_t gh_time_elapsed();
void	gh_transform_edges(const mat4 *tf, vec3 *edge, const int num_edges);
void	gh_transform_vec3(const mat4 *tf, vec3 *v, const int num_points);

#endif /* !_GAME_HELPER_H_ */

