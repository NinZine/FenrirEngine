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
#include "game.h"
#include "game_entity.h"
#include "game_helper.h"
#include "vec3.h"

#define ADD_ACTION(x) \
{#x, &action_##x##_attr, &action_##x }

#define DEF_ACTION(x) \
static void \
action_##x##_attr(b_attribute **b, unsigned int *prev_attrs); \
static void \
action_##x (void *self, b_attribute *b, unsigned int attrs)

#define DECL_ACTION_ATTR(x) \
void \
action_##x##_attr(b_attribute **b, unsigned int *prev_attrs)

#define DECL_ACTION(x) \
void \
action_##x (void *self, b_attribute *b, unsigned int attrs)

DEF_ACTION(move);
DEF_ACTION(shoot);

static struct b_action_info action[] = {
	ADD_ACTION(move),
	ADD_ACTION(shoot),
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

DECL_ACTION(move)
{
	b_attribute *direction,
				*speed;
	gh_rigid_body	*tmp;
	vec3		v;
	quat		rot;

	direction = b_find_attribute("direction", b, attrs);
	speed = b_find_attribute("speed", b, attrs);

	if (!direction || !speed) {
		return;
	}

	tmp = game_get_rigidbody(((g_entity*)self)->rb);
	v = *(vec3*)direction->value;
	v = vec3_normalize(&v);
	rot.w = -gh_rad2deg(atan2(v.y, v.x));
	
	v = vec3_mul(&v, *(float*)speed->value);
	tmp->linear_velocity = v;
	rot.x = rot.y = 0.f;
	rot.z = 1.f;
	tmp->rotation = quat_from_axis(&rot);
}

DECL_ACTION_ATTR(move)
{
	
	b_add_attribute(b, prev_attrs, "speed", 'f', 1.f);
}

DECL_ACTION(shoot)
{
	g_entity *bullet;
	
	/* Spawn particle / entity *//*
	bullet = malloc(sizeof(g_entity));
	gh_create_model();
	gh_create_rigidbody();
	gh_create_behavior();
	bullet = gh_create_entity(model, rigidbody, NULL);*/
}

DECL_ACTION_ATTR(shoot)
{
}
