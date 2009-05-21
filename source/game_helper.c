/*
 *  GameHelper.c
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/14/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#include <sys/time.h>

#include <stdlib.h>

#include "game_helper.h"

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
		dest->object[i].linear_velocity = src->object[i].linear_velocity;
		dest->object[i].angular_velocity = src->object[i].angular_velocity;
	}
}

float_t
gh_time_elapsed()
{
	static struct timeval start = {0,0};
	struct timeval counter;
	
	if (0 == start.tv_sec && 0 == start.tv_usec)
	{
		gettimeofday(&start, 0);
		return 0.f;
	}
	
	gettimeofday(&counter, 0);
	counter.tv_sec = counter.tv_sec - start.tv_sec;
	counter.tv_usec = counter.tv_usec - start.tv_usec;
	
	float_t tmp = ((counter.tv_sec * 1000000) + counter.tv_usec) / 1000000.f;
    return tmp;
}
