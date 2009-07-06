/*-
	 Copyright (C) 2009 Andreas Kroehnke <ninzine@indraz.com>
	 
	 This program is free software; you can redistribute it and/or
	 modify it under the terms of the GNU General Public License
	 as published by the Free Software Foundation; either version 2
	 of the License, or (at your option) any later version.
	 
	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <string.h>

#include "actions.h"
#include "array.h"
#include "behavior.h"
#include "game_entity.h"
#include "game_helper.h"
#include "vec3.h"

static void action_move(void *self, b_attribute *b, unsigned int attrs);
static unsigned int action_move_attr(b_attribute **b,
	unsigned int prev_attrs);

static struct b_action_info action[] = {
	{"move", action_move_attr, action_move},
};
static unsigned int actions = sizeof(action) / sizeof(struct b_action_info);

void
b_get_action(const char *name, struct b_action_info **info)
{
	int i;
	
	for (i = 0; i < actions; ++i) {
		if (0 == strcmp(name, action[i].name)) {
			*info = &action[i];
			break;
		}
	}
}

void
action_move(void *self, b_attribute *b, unsigned int attrs)
{
	b_attribute *direction,
				*speed;
	g_entity	*tmp;
	vec3		v;
	quat		rot;

	direction = b_find_attribute("direction", b, attrs);
	speed = b_find_attribute("speed", b, attrs);

	if (!direction || !speed) {
		return;
	}

	tmp = (g_entity*)self;
	v = *(vec3*)direction->value;
	v = vec3_normalize(&v);
	rot.w = -gh_rad2deg(atan2(v.y, v.x));
	
	v = vec3_mul(&v, *(float*)speed->value);
	tmp->rb->linear_velocity = v;
	rot.x = rot.y = 0.f;
	rot.z = 1.f;
	tmp->rb->rotation = quat_from_axis(&rot);
}

unsigned int
action_move_attr(b_attribute **b, unsigned int prev_attrs)
{
	
	gh_array_resize((void**)b, prev_attrs, sizeof(b_attribute), 1);
	b_create_attribute(&(*b)[prev_attrs], "speed", 'f', 1.f);

	return 1;
}

