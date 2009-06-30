/*-
 * License
 */

#ifndef _GAME_HELPER_H_
#define _GAME_HELPER_H_

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "mat4.h"
#include "quat.h"
#include "vec3.h"


enum gh_input_type { GHI_MOVE, GHI_MOVE_STOP, };

struct gh_input {
	enum gh_input_type	type;
	void				*data;
};

struct gh_rigid_body {
	int32_t id;
	vec3	position;
	quat	rotation;
	vec3	linear_velocity;
	vec3	angular_velocity;
};

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
void	gh_project_vec3(const vec3 *axis, const vec3 *points, const int sz_points,
			float_t *min, float_t *max);
float_t gh_time_elapsed();

#endif /* !_GAME_HELPER_H_ */

