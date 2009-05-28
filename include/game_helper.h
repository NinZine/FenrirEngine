/*
 *  GameHelper.h
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/14/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _GAME_HELPER_H_
#define _GAME_HELPER_H_

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "vec3.h"

struct gh_rigid_body {
	int32_t id;
	vec3	position;
	/*quat	rotation;*/
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

void	gh_copy_state(struct gh_state *dest, struct gh_state *src,
				bool use_malloc);
float_t gh_time_elapsed();

#endif /* !_GAME_HELPER_H_ */

